#ifndef GDALCONNECTOR_GLOBAL_H
#define GDALCONNECTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GDALCONNECTOR_LIBRARY)
#  define GDALCONNECTORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GDALCONNECTORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GDALCONNECTOR_GLOBAL_H
