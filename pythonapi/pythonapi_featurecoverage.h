#ifndef PYTHONAPI_FEATURECOVERAGE_H
#define PYTHONAPI_FEATURECOVERAGE_H

#include "pythonapi_coverage.h"
#include "pythonapi_feature.h"
#include "pythonapi_geometry.h"

namespace Ilwis {
    class FeatureCoverage;
    typedef IlwisData<FeatureCoverage> IFeatureCoverage;
}

namespace pythonapi {

    class FeatureIterator;

    class FeatureCoverage : public Coverage{
        friend class FeatureIterator;
    public:
        FeatureCoverage();
        FeatureCoverage(const char* resource);
        unsigned int featureCount() const;
        Feature newFeature(Geometry& geometry);
        static FeatureCoverage* toFeatureCoverage(Object *obj);
    };
}

#endif // PYTHONAPI_FEATURECOVERAGE_H
