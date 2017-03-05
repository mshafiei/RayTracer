#pragma once
#include "rt/coordmappers/coordmapper.h"
namespace rt {

    class Intersection;
    class EnvironmentCoordMapper : public CoordMapper {
    public:
        EnvironmentCoordMapper();
        virtual Point getCoords(const Intersection& hit) const;
    };

}