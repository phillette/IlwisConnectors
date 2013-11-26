#ifndef GDALFEATURECONNECTOR_H
#define GDALFEATURECONNECTOR_H

namespace Ilwis{
namespace Gdal {

struct SourceHandles{
    SourceHandles(OGRDataSourceH source=0, const std::vector<OGRLayerH>& layer=std::vector<OGRLayerH>()) : _source(source), _layers(layer) {}
    OGRDataSourceH _source;
    std::vector<OGRLayerH> _layers;
};
typedef std::vector<SourceHandles> FeatureSetHandles;

class GdalFeatureConnector : public CoverageConnector{
public:
    GdalFeatureConnector(const Ilwis::Resource &resource, bool load=true);

    bool loadMetaData(IlwisObject* data);
    bool loadBinaryData(IlwisObject* data);
    bool store(IlwisObject *obj, int);

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
    Ilwis::IlwisObject *create() const;
protected:
    enum OutputState { osIndex2Layer=1,osType2Layer=2, osLayer2DataSource=4};
    struct FillerColumnDef{
        FillerColumnDef(QVariant(GdalFeatureConnector::* func)(OGRFeatureH,  int, SPRange), int i, SPRange r): fillFunc(func), index(i), range(r){}
        QVariant (GdalFeatureConnector::* fillFunc)(OGRFeatureH,  int, SPRange);
        int index;
        SPRange range;
    };
    QVariant fillEmptyColumn(OGRFeatureH, int, SPRange);
    QVariant fillStringColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    QVariant fillIntegerColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    QVariant fillDoubleColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    QVariant fillDateTimeColumn(OGRFeatureH featureH, int colIntex, SPRange range);
    IlwisTypes translateOGRType(OGRwkbGeometryType type) const;
    std::vector<SPFeatureI> fillFeature(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    std::vector<SPFeatureI> fillPoint(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    std::vector<SPFeatureI> fillLine(FeatureCoverage *fcoverage, OGRGeometryH geometry) const;
    std::vector<SPFeatureI> fillPolygon(FeatureCoverage *fcoverage, OGRGeometryH geometry, OGRwkbGeometryType type) const;
private:
    OGRSFDriverH _driver;

    OGRDataSourceH createFileBasedDataSource(const QString &postfix, const QFileInfo &fileinfo) const;
    OGRLayerH createLayer(const QString &name, OGRwkbGeometryType type, OGRSpatialReferenceH srs, OGRDataSourceH source);
    bool oneLayerPerFeatureType(const IFeatureCoverage &features);
    bool createAttributes(const ITable &tbl, OGRLayerH layer, const std::vector<OGRFieldDefnH> &fielddefs);
    bool loadDriver();
    OGRwkbGeometryType ilwisType2GdalFeatureType(IlwisTypes tp);
};
}
}
#endif // GDALFEATURECONNECTOR_H


