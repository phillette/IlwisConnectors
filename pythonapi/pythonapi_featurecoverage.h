#ifndef PYTHONAPI_FEATURECOVERAGE_H
#define PYTHONAPI_FEATURECOVERAGE_H

#include "pythonapi_coverage.h"

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
        static FeatureCoverage* toFeatureCoverage(Object *obj);
    };
}

#endif // PYTHONAPI_FEATURECOVERAGE_H