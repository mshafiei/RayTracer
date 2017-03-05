#include "rt/coordmappers/world.h"
using namespace rt;

WorldMapper::WorldMapper()
{

}
WorldMapper::WorldMapper(const Float4& scale) :scale(scale)
{
}

Point WorldMapper::getCoords(const Intersection& hit) const
{     
    Point hitP = hit.hitPoint();
    return Point(hitP.x * scale.x, hitP.y * scale.y, hitP.z * scale.z);
}