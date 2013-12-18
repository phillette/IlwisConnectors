#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"

#include "../../IlwisCore/core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometry.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"

#include "pythonapi_geometry.h"
#include "pythonapi_feature.h"

namespace pythonapi{

Geometry::Geometry(const char *wkt): _standalone(true), _wkt(std::string(wkt)){
}

Geometry::Geometry(Feature *feature, int index): _standalone(false),_feature(feature), _index(index){
}

Geometry::~Geometry(){
}

bool Geometry::within(const Geometry &geometry) const{
    return this->ptr().within(geometry.ptr());
}

bool Geometry::contains(const Geometry &geometry) const{
    return geometry.within(*this);
}

bool Geometry::__bool__() const{
    if (this->_standalone){
        return !_wkt.empty();
    }else{
        return this->_feature != nullptr && (bool)this->_feature && this->_feature->__bool__() && this->_feature->ptr()->geometry(this->_index).isValid();
    }
}

const char* Geometry::__str__(){
    if (this->__bool__())
        return this->toWKT();
    else
        return "invalid Geometry!";
}

IlwisTypes Geometry::ilwisType(){
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Feature!"));
    return this->ptr().geometryType();
}

bool Geometry::fromWKT(const char* wkt){
    return this->_feature->ptr()->geometry().fromWKT(QString(wkt));
}

const char *Geometry::toWKT(){
    if(this->_standalone)
        return _wkt.c_str();
    else
        return this->ptr().toWKT().toLocal8Bit();
}

Ilwis::Geometry &Geometry::ptr() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid Geometry!"));
    return this->_feature->ptr()->geometry(this->_index);
}

Coordinate::Coordinate(double x, double y): _2d(true), _data(new Ilwis::Point3D<double>(x,y,0)){
}

Coordinate::Coordinate(double x, double y, double z): _2d(false), _data(new Ilwis::Point3D<double>(x,y,z)){
}

const char* Coordinate::__str__(){
    if (this->_2d)
        return QString("coordinate(%1,%2)").arg(this->_data->x()).arg(this->_data->y()).toLocal8Bit();
    else
        return QString("coordinate(%1,%2,%3)").arg(this->_data->x()).arg(this->_data->y()).arg(this->_data->z()).toLocal8Bit();
}

Ilwis::Point3D<double>& Coordinate::data(){
    return (*this->_data);
}

Pixel::Pixel(qint32 x, qint32 y): _x(x), _y(y){
}

const char* Pixel::__str__(){
    return QString("pixel(%1,%2)").arg(this->_x).arg(this->_y).toLocal8Bit();
}

Voxel::Voxel(const Ilwis::Point3D<qint32> &vox): _data(new Ilwis::Point3D<>(vox)){
}

Voxel::Voxel(qint32 x, qint32 y, qint32 z): _data(new Ilwis::Point3D<>(x,y,z)){
}

const char* Voxel::__str__(){
    //TODO kept (3D)pixel as string representation for use in Engine.do(..) better change to voxel(,,,)
    return QString("pixel(%1,%2,%3)").arg(this->_data->x()).arg(this->_data->y()).arg(this->_data->z()).toLocal8Bit();
}

Ilwis::Point3D<qint32> &Voxel::data() const{
    return (*this->_data);
}

Box::Box():_data(new Ilwis::Box3D<>()){
}

Box::Box(const Ilwis::Box3D<qint32>& box): _data(new Ilwis::Box3D<>(box)){
}

Box::Box(const char *envelope): _data(new Ilwis::Box3D<>(envelope)){
}

Box::Box(const Voxel &min, const Voxel &max): _data(new Ilwis::Box3D<>(min.data(), max.data())){
}

const char *Box::__str__(){
    return this->data().toString().toLocal8Bit();
}

Size Box::size(){
    return Size(this->data().size());
}

Ilwis::Box3D<qint32> &Box::data() const{
    return (*this->_data);
}

Size::Size(qint32 xsize, qint32 ysize, qint32 zsize):_data(new Ilwis::Size(xsize,ysize,zsize)){
}

Size::Size(const Ilwis::Size &size):_data(new Ilwis::Size(size)){
}

Size* Size::operator+=(const Size &sz){
    return new Size(this->data() += sz.data());
}

Size* Size::operator-=(const Size &sz){
    return new Size(this->data() -= sz.data());
}

Size* Size::operator*=(double f){
    return new Size(this->data() *= f);
}

qint32 Size::xsize() const{
    return this->data().xsize();
}

qint32 Size::ysize() const{
    return this->data().ysize();
}

qint32 Size::zsize() const{
    return this->data().zsize();
}

void Size::setXsize(qint32 x){
    this->data().xsize(x);
}

void Size::setYsize(qint32 y){
    this->data().ysize(y);
}

void Size::setZsize(qint32 z){
    this->data().zsize(z);
}

quint64 Size::linearSize() const{
    return this->data().totalSize();
}

bool Size::contains(qint32 x, qint32 y, qint32 z) const{
    return this->data().contains(x,y,z);
}

std::string Size::__str__(){
    return QString("Size(%1 %2 %3)").arg(this->data().xsize()).arg(this->data().ysize()).arg(this->data().zsize()).toStdString();
}

Ilwis::Size &Size::data() const{
    return (*this->_data);
}



}//namespace pythonapi
