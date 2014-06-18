#ifndef PYTHONAPI_DataDefinition_H
#define PYTHONAPI_DataDefinition_H

#include <memory>
#include "kernel.h"

namespace Ilwis{
class DataDefinition;
class Domain;
class Range;
}

namespace pythonapi {

class Domain;
class Range;

class DataDefinition{

    friend class RasterCoverage;

public:
    DataDefinition();
    DataDefinition(Ilwis::DataDefinition* datdef);
    ~DataDefinition();
    DataDefinition(Domain* dm, const Range &rng);
    DataDefinition(const DataDefinition &datdef);
    bool __bool__() const;
    void range(const Range &rng);
    void domain(Domain* dm);
    bool isCompatibleWith(const DataDefinition& datdef);
    DataDefinition* merge(const DataDefinition& datdef1, const DataDefinition& datdef2);
    DataDefinition* __set__(const DataDefinition& datdef);

    Range* range() const;
    Domain* domain() const;

protected:
    Ilwis::DataDefinition& ptr() const;
    std::shared_ptr<Ilwis::DataDefinition> _ilwisDatadef;
};

}

#endif // PYTHONAPI_DataDefinition_H