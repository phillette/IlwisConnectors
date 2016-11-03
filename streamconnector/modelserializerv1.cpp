#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationsetup.h"
#include "model.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"
#include "modelserializerV1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *ModelSerializerV1::create(QDataStream& stream)
{
    return new ModelSerializerV1(stream);
}

ModelSerializerV1::ModelSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool ModelSerializerV1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Model *model = static_cast<Model *>(obj);

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    std::unique_ptr<DataInterface> wfstreamer(factory->create(Version::interfaceVersion, itWORKFLOW,_stream));
    if ( !wfstreamer)
        return false;

    _stream << model->workflowCount();
    for(int i=0; i < model->workflowCount(); ++i){
        if(!wfstreamer->store(model->workflow(i).ptr(), options))
            return false;
    }
    _stream << model->analysisCount();
    for(int i=0; i < model->analysisCount(); ++i){
        model->analysisPattern(i)->store(_stream);
    }

    _stream << model->applicationCount();
    for(int i=0; i < model->applicationCount(); ++i){
        model->application(i)->store(_stream);
    }

    return true;
}

bool ModelSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    Model *model = static_cast<Model *>(obj);

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    std::unique_ptr<DataInterface> wfstreamer(factory->create(Version::interfaceVersion, itWORKFLOW,_stream));
    if ( !wfstreamer)
        return false;

    qint32 count;

    _stream >> count;
    for(int i=0; i < count; ++i){

        quint64 type;
        QString version, url;
        _stream >> url;
        _stream >> type;
        _stream >> version;
        IWorkflow systemWf = makeSystemObject<IWorkflow>(url);
        IWorkflow wf (type);
        if(!wfstreamer->loadData(wf.ptr(), options))
            return false;
        model->addWorklfow(systemWf.isValid() ? systemWf :wf );
    }
    _stream >> count;
    for(int i=0; i < count; ++i){
        //AnalysisPattern *pattern = new AnalysisPattern();
        //pattern->load(_stream);
        //model->addAnalysisPattern(pattern);
    }

    _stream >> count;
    for(int i=0; i < count; ++i){
        ModelApplication *app = new ModelApplication();
        app->load(_stream);
        model->addApplication(app);
    }

    return true;
}

