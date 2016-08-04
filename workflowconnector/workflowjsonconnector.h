#ifndef WORKFLOWJSONCONNECTOR_H
#define WORKFLOWJSONCONNECTOR_H

#include <QBuffer>

namespace Ilwis {

class Workflow;

namespace WorkflowConnector {

    class WorkflowJSONConnector : public IlwisObjectConnector
    {
    public:
        WorkflowJSONConnector(const Ilwis::Resource &resource, bool,const IOOptions& options=IOOptions() );

        bool loadMetaData(IlwisObject*, const IOOptions & );
        bool loadData(IlwisObject*, const IOOptions& options = IOOptions());
        bool store(IlwisObject *, const IOOptions& options = IOOptions() );
        QString type() const;
        bool isReadOnly() const;
        Ilwis::IlwisObject *create() const;
        QString provider() const;
        QVariant getProperty(const QString& ) const;

        static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true,const IOOptions& options=IOOptions());

    private:
        QJsonObject createJSONWorkflow(const Resource & res);
        QJsonObject createJSONWorkflowMetaData(const Resource& res);
        QJsonObject createJSONOperationMetadata(const Resource &res);
        QJsonObject createJSONOperationList(const Resource &res);
        QJsonArray createJSONOperationInputList(Workflow* workflow, const OVertex v);
        QJsonArray createJSONOperationOutputList(Ilwis::Workflow *workflow, const Ilwis::OVertex v);
        QJsonArray createJSONOperationConnectionList(Ilwis::Workflow *workflow);
        bool openTarget();

        std::unique_ptr<QIODevice> _datasource;

        JsonConfig _config;

        QByteArray _data;
        QMap<OVertex, QStringList> _nodeExecutionContext;   // contains a list of completed operations
        QMap<OVertex, QStringList> _inputArgs;
        OperationExpression _expression;
        QStringList _outputNames;
        QMap<QString, QString> _layer2LocalLUT;

        void setInputParm(const QString baseName, const Ilwis::SPOperationParameter parm, QJsonObject &input);

        void parseOutputNames(OperationExpression expression);
        void parseInputNodeArguments(Ilwis::Workflow *workflow, const QList<OVertex> &inputNodes);
        void addGeneratedNames(const OVertex &v, QStringList& names, const Ilwis::IOperationMetaData &meta);
        void writeWMSLocalLUT(QString filename);
    };

}
}

#endif // WORKFLOWJSONCONNECTOR_H
