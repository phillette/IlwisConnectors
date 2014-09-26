#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/basetable.h"

#include "pythonapi_table.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_feature.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_error.h"
#include "pythonapi_columndefinition.h"

namespace pythonapi {

    Table::Table(){        
    }

    Table::Table(Ilwis::ITable *table):IlwisObject(new Ilwis::IIlwisObject(*table)){
        delete table;
    }

    Table::Table(std::string resource, const IOOptions& opt){
        Ilwis::ITable t(QString::fromStdString(resource), itTABLE, opt.ptr());
        if (t.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(t));
    }

    quint32 Table::recordCount() const{
        return this->ptr()->as<Ilwis::Table>()->recordCount();
    }

    quint32 Table::columnCount() const{
        return this->ptr()->as<Ilwis::Table>()->columnCount();
    }

    bool Table::addColumn(const std::string& name, const std::string& domain){
        return this->ptr()->as<Ilwis::Table>()->addColumn(QString::fromStdString(name),QString::fromStdString(domain));
    }

    bool Table::addColumn(ColumnDefinition& coldef){
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        return this->ptr()->as<Ilwis::Table>()->addColumn(*ilwDef);
    }

    PyObject* Table::columns() const{
        Ilwis::ITable tbl = this->ptr()->as<Ilwis::Table>();
        PyObject* list = newPyTuple(tbl->columnCount());
        for(int i = 0; i < tbl->columnCount(); i++){
            if (!setTupleItem(list, i, PyUnicodeFromString(tbl->columndefinition(i).name().toStdString().data())))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of columns").arg(tbl->columndefinition(i).name()));
        }
        return list;
    }

    PyObject* Table::select(const std::string& conditions) const{
        std::vector<quint32> lst = this->ptr()->as<Ilwis::Table>()->select(QString::fromStdString(conditions));
        PyObject* list = newPyTuple(lst.size());
        int i = 0;
        for(auto it = lst.begin(); it != lst.end(); it++){
            if (!setTupleItem(list, i++, PyLongFromSize_t(*it)))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of columns").arg(*it));
        }
        return list;
    }

    qint32 Table::columnIndex(const std::string& name) const{
        quint32 idx = this->ptr()->as<Ilwis::Table>()->columnIndex(QString::fromStdString(name));
        if (idx == Ilwis::iUNDEF)
            return Ilwis::iUNDEF;
        else
            return idx;
    }

    PyObject* Table::cell(const std::string& name, quint32 rec){
        QVariant ret = this->ptr()->as<Ilwis::Table>()->cell(QString::fromStdString(name), rec,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute '%1' found or record %2 out of bound").arg(name.c_str()).arg(rec).toStdString());
        return QVariant2PyObject(ret);
    }

    PyObject* Table::cell(quint32 colIndex, quint32 rec){
        QVariant ret = this->ptr()->as<Ilwis::Table>()->cell(colIndex, rec,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute in '%1.' column found or record %2 out of bound").arg(colIndex).arg(rec).toStdString());
        return QVariant2PyObject(ret);
    }

    void Table::setCell(const std::string& name, quint32 rec, const PyObject* value){
        QVariant* v = PyObject2QVariant(value);
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, *v);
        delete v;
    }

    void Table::setCell(quint32 colIndex, quint32 rec, const PyObject* value){
        QVariant* v = PyObject2QVariant(value);
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, *v);
        delete v;
    }

    void Table::setCell(const std::string& name, quint32 rec, qint64 value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, qint64 value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    void Table::setCell(const std::string& name, quint32 rec, std::string value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value.c_str()));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, std::string value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value.c_str()));
    }

    void Table::setCell(const std::string& name, quint32 rec, double value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, double value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    PyObject* Table::column(const std::string& name) const{
        return StdVectorOfQVariant2PyTuple(this->ptr()->as<Ilwis::Table>()->column(QString::fromStdString(name)));
    }

    PyObject* Table::column(quint32 columnIndex) const{
        return StdVectorOfQVariant2PyTuple(this->ptr()->as<Ilwis::Table>()->column(columnIndex));
   }

    PyObject* Table::record(quint32 rec) const{
        return StdVectorOfQVariant2PyTuple(this->ptr()->as<Ilwis::Table>()->record(rec));
    }

    Table* Table::toTable(Object *obj){
        Table* ptr = dynamic_cast<Table*>(obj);
        if(!ptr)
            throw InvalidObject("cast to Table not possible");
        return ptr;
    }

    ColumnDefinition Table::columnDefinition(const std::string& name) const{
        QString qName;
        qName = qName.fromStdString(name);
        Ilwis::ColumnDefinition* coldef = new Ilwis::ColumnDefinition(this->ptr()->as<Ilwis::Table>()->columndefinition(qName));
        return ColumnDefinition(coldef);
    }

    ColumnDefinition Table::columnDefinition(quint32 index) const{
        Ilwis::ColumnDefinition* coldef = new Ilwis::ColumnDefinition(this->ptr()->as<Ilwis::Table>()->columndefinition(index));
        return ColumnDefinition(coldef);
    }

    void Table::setColumnDefinition(ColumnDefinition& coldef){
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        this->ptr()->as<Ilwis::Table>()->columndefinition(*ilwDef);
    }

    void Table::setColumnDefinition(const std::string &name, ColumnDefinition* coldef){
        QString qName;
        qName = qName.fromStdString(name);
        Ilwis::ColumnDefinition& ilwdef = this->ptr()->as<Ilwis::Table>()->columndefinitionRef(qName);
        Ilwis::ColumnDefinition* newDef = coldef->ptr().get();
        ilwdef = Ilwis::ColumnDefinition(*newDef, ilwdef.columnindex());
    }

    void Table::setColumnDefinition(quint32 index, ColumnDefinition *coldef){
        Ilwis::ColumnDefinition& ilwdef = this->ptr()->as<Ilwis::Table>()->columndefinitionRef(index);
        Ilwis::ColumnDefinition* newDef = coldef->ptr().get();
        ilwdef = Ilwis::ColumnDefinition(*newDef, ilwdef.columnindex());
    }


} // namespace pythonapi
