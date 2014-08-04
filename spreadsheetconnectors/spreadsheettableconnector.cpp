#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domainitem.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "spreadsheet.h"
#include "odsformat.h"
#include "xlsformat.h"
#include "xlsxformat.h"
#include "spreadsheettableconnector.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

const std::vector<QString> SpreadSheetTableConnector::_suffixes = {"xls", "ods", "xlsx"};

SpreadSheetTableConnector::SpreadSheetTableConnector(const QString& file) : IlwisObjectConnector(Resource(file,itTABLE), true, IOOptions()){
    QFileInfo path(file);
     if ( path.suffix().toLower() == "ods"){
        _spreadsheet.reset( new ODSFormat());
    } else if ( path.suffix().toLower() == "xls"){
        _spreadsheet.reset( new XLSFormat());
    } else if ( path.suffix().toLower() == "xlsx"){
        _spreadsheet.reset( new XLSXFormat());
    }
     if ( _spreadsheet) {
         _spreadsheet->openSheet(file);
     }

}

SpreadSheetTableConnector::SpreadSheetTableConnector(const Ilwis::Resource &resource, bool load, const Ilwis::IOOptions &options) :  IlwisObjectConnector(resource, load, options)
{
    QFileInfo odsinfo = resource.toLocalFile();
    QString sheetName;
    if ( !knownSuffix(odsinfo.suffix())){
        int index  = odsinfo.absoluteFilePath().lastIndexOf("/");
        sheetName = odsinfo.absoluteFilePath().mid(index + 1);
        odsinfo = QFileInfo(odsinfo.absolutePath());
    }
    if ( odsinfo.suffix().toLower() == "ods"){
        _spreadsheet.reset( new ODSFormat());
    } else if ( odsinfo.suffix().toLower() == "xls"){
        _spreadsheet.reset( new XLSFormat());
    } else if ( odsinfo.suffix().toLower() == "xlsx"){
        _spreadsheet.reset( new XLSXFormat());
    }

}

void SpreadSheetTableConnector::setColumnDefinitions(Table * tbl, const std::vector<ColInfo>& columnType)
{
    _validColumns.resize(columnType.size(), false);
    int count = 0;
    for(auto inf : columnType){
        ColumnDefinition coldef;
        QString colname =  inf._name == sUNDEF ? QString("column_%1").arg(_validColumnCount) : inf._name;

        if ( hasType(inf._type, itDOUBLE | itFLOAT)) {
            coldef = ColumnDefinition(colname,DataDefinition({"value"}), _validColumnCount);
        } else if ( hasType(inf._type, itINTEGER)){
            coldef = ColumnDefinition(colname,DataDefinition({"integer"}), _validColumnCount);
        } else if ( hasType(inf._type, itDATETIME)){
            coldef = ColumnDefinition(colname,DataDefinition({"time"}), _validColumnCount);
        } else if ( hasType(inf._type,itSTRING)) {
            coldef = ColumnDefinition(colname,DataDefinition({"text"}), _validColumnCount);
        }else{
            continue;
        }
        _validColumns[count++] = true;
        ++_validColumnCount;

        tbl->addColumn(coldef);
    }
}

bool SpreadSheetTableConnector::loadMetaData(IlwisObject *data, const Ilwis::IOOptions &options)
{
    _spreadsheet->openSheet(_resource.toLocalFile());
    if (!_spreadsheet->isValid())
        return false;

    bool ok;
    int rowShift = 0;
    quint32 headerline =  options["headerline"].toUInt(&ok);
    if ( ok){
        _headerline = headerline;
        rowShift = 1;
    }
    std::vector<ColInfo> columnType(_spreadsheet->columnCount());

    int rowCount = std::min(_spreadsheet->rowCount(), (quint32)20);
    for(int row = 0; row < rowCount; ++row){
        if (! _spreadsheet->isRowValid( row))
            continue;

        int columnCount = _spreadsheet->columnCount();
        for( int col = 0; col < columnCount; ++col){
            if (!_spreadsheet->isCellValid(col, row))
                continue;

            if ( rowShift == 1 && row == _headerline){
                columnType[col]._name = _spreadsheet->cellValue(col, row).toString();
                continue;
            }
            columnType[col]._type = _spreadsheet->cellValueType(col, row);
        }

    }

    Table * tbl = static_cast<Table *>(data);
    setColumnDefinitions(tbl,columnType);

    return true;
}

bool SpreadSheetTableConnector::loadData(IlwisObject *object, const IOOptions &options)
{
    _spreadsheet->openSheet(_resource.toLocalFile());
    if (!_spreadsheet->isValid())
        return false;


    int rowShift = _headerline != iUNDEF ? 1 : 0;

    std::vector<std::vector<QVariant>> data(_spreadsheet->rowCount() - rowShift);

    // number of columns per row is equal to the valid columns we have seen
    for(auto& row : data)
        row.resize(_validColumnCount);

    for(int row = 0; row < _spreadsheet->rowCount(); ++row){
        if (! _spreadsheet->isRowValid( row))
            continue;

        // skip the row that contains the header
        if ( row == _headerline)
            continue;

        int columnCount = _spreadsheet->columnCount( );
        int tableColumn = 0; // counts the real columns as they will appear i
        for( int col = 0; col < columnCount; ++col){
            if ( _validColumns[col]){ // skip columns that were not valid
                if ( _spreadsheet->isCellValid(col, row)) {
                    data[row - rowShift][tableColumn] = _spreadsheet->cellValue(col, row);
                }
                ++tableColumn;
            }
        }
    }

    Table * tbl = static_cast<Table *>(object);

    for(int rec = 0; rec < _spreadsheet->rowCount() - rowShift; ++rec){
        if ( data[rec].size() > 0)
            tbl->record(rec, data[rec]);
    }
    _binaryIsLoaded = true;
    return true;

}

bool SpreadSheetTableConnector::store(IlwisObject *object, int ){
    _spreadsheet->openSheet(_resource.toLocalFile(), false);
    if (!_spreadsheet->isValid())
        return false;

    Table * tbl = static_cast<Table *>(object);
    bool allDeafultNames = true;
    for(int col = 0; col < tbl->columnCount() && allDeafultNames; ++col){
        allDeafultNames &= tbl->columndefinition(col).name().indexOf("column_") == 0;
    }
    if ( !allDeafultNames){
        for(int col = 0; col < tbl->columnCount(); ++col){
            _spreadsheet->cellValue(col, 0, tbl->columndefinition(col).name(), true );
        }
    }
    int rowStart = allDeafultNames ? 0 : 1;
    for(int row = 0; row < tbl->recordCount(); ++row){
        for(int col = 0; col < tbl->columnCount(); ++col) {
            if ( hasType(tbl->columndefinition(col).datadef().domain()->ilwisType(), itITEMDOMAIN)){
                _spreadsheet->cellValue(col, row + rowStart, tbl->cell(col, row,false), true);
            } else {
                _spreadsheet->cellValue(col, row + rowStart, tbl->cell(col, row), true);
            }
        }
    }

    QString error = _spreadsheet->storeSheet(_resource.toLocalFile());
    if ( !error.isEmpty()){
        kernel()->issues()->log(error);
        return false;
    }



    return true;
}

QString SpreadSheetTableConnector::type() const
{
    return "Table";
}

IlwisObject *SpreadSheetTableConnector::create() const
{
    return new FlatTable(_resource);
}

QString SpreadSheetTableConnector::provider() const
{
    return "spreadsheets";
}

const std::unique_ptr<SpreadSheet> &SpreadSheetTableConnector::spreadsheet() const
{
    return _spreadsheet;
}

bool SpreadSheetTableConnector::isValid() const
{
    return _spreadsheet && _spreadsheet->isValid();
}

bool SpreadSheetTableConnector::knownSuffix(const QString &suffix){
    return std::find(_suffixes.begin(), _suffixes.end(), suffix.toLower()) != _suffixes.end();
}

ConnectorInterface *SpreadSheetTableConnector::create(const Ilwis::Resource &resource, bool load, const Ilwis::IOOptions &options)
{
    return new SpreadSheetTableConnector(resource, load, options);
}

