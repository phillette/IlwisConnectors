#include <QSqlDatabase>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "resource.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalog.h"

#include "postgresqldatabaseutil.h"
#include "postgresqlcatalogexplorer.h"

using namespace Ilwis;
using namespace Postgresql;

REGISTER_CATALOGEXPLORER(PostgresqlCatalogExplorer)

CatalogExplorer *PostgresqlCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new PostgresqlCatalogExplorer(resource, options);
}

PostgresqlCatalogExplorer::PostgresqlCatalogExplorer(const Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{
    int index = resource.url(true).toString().indexOf("postgresql://");
    if ( index == 0) {
        QString rest = resource.url(true).toString().mid(13);
        QStringList parts = rest.split("/");
        QStringList hostport = parts[0].split(":");
        ioOptionsRef().addOption("port", hostport[1]);
        ioOptionsRef().addOption("host", hostport[0]);
        ioOptionsRef().addOption("database", parts[1]);
        ioOptionsRef().addOption("pg.schema", parts[2]);
        QStringList userpass = parts[3].split("&");
        ioOptionsRef().addOption("pg.username", userpass[0].split("=")[1]);
        ioOptionsRef().addOption("pg.password", userpass[1].split("=")[1]);

    }
}

PostgresqlCatalogExplorer::~PostgresqlCatalogExplorer()
{
}

std::vector<Resource> PostgresqlCatalogExplorer::loadItems(const IOOptions &options)
{
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    QString schema("public");
    if (iooptions.contains("pg.schema")) {
        schema = iooptions["pg.schema"].toString();
    }
    QString sqlBuilder;
    sqlBuilder.append("SELECT ");
    sqlBuilder.append(" meta.tablename, count(geom.typname) as hasGeometry ");
    sqlBuilder.append(" FROM ");
    sqlBuilder.append(" pg_catalog.pg_tables AS meta,pg_catalog.pg_type AS geom ");
    sqlBuilder.append(" WHEREGROUP ");
    sqlBuilder.append(" meta.schemaname = '").append(schema).append("' ");
    sqlBuilder.append(" AND ");
    sqlBuilder.append(" geom.typname = 'geometry' ");
    sqlBuilder.append("  BY ");
    sqlBuilder.append(" meta.tablename;");
    //qDebug() << "SQL: " << sqlBuilder;

    std::vector<Resource> resources;
    PostgresqlDatabaseUtil pgUtil(source(), iooptions);
    QSqlQuery query = pgUtil.doQuery(sqlBuilder, "exploreitems");

    QString parentDatasourceNormalized = source().url().toString();
    parentDatasourceNormalized = !parentDatasourceNormalized.endsWith("/")
            ? parentDatasourceNormalized.append("/")
            : parentDatasourceNormalized;

    while (query.next()) {
        QString tablename = query.value(0).toString();
        bool hasGeometry = query.value(1).toBool();
        if (tablename == "spatial_ref_sys") {
            continue; // skip system table
        }
        QString resourceId = parentDatasourceNormalized;
        //resourceId.append(schema);
       // resourceId.append("/");
        resourceId.append(tablename);
        //qDebug() << "create new resource: " << resourceId;

        IlwisTypes mainType;
        IlwisTypes extTypes;
        if ( hasGeometry) {
            mainType = itFEATURE;
            extTypes = itFLATTABLE;
        } else {
            mainType = itFLATTABLE;
        }

        QUrl url(resourceId);
        Resource table(url, mainType);
        table.setExtendedType(extTypes);
        resources.push_back(table);
    }

    return resources;
}

bool PostgresqlCatalogExplorer::canUse(const Resource &resource) const
{
    if (resource.url().scheme() != "postgresql")
        return false;
    if ( resource.ilwisType() != itCATALOG)
        return false;
    QString dbName = resource.url().path();
    return dbName.startsWith("/")
            ? dbName.length() > 1
            : dbName.length() > 0;
}

QString PostgresqlCatalogExplorer::provider() const
{
    return "postgresql";
}

