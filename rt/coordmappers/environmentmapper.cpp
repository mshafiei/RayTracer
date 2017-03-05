#include "rt/coordmappers/environmentmapper.h"
#include "core/assert.h"
#include "rt/intersection.h"

using namespace rt;

EnvironmentCoordMapper::EnvironmentCoordMapper()
{

}


Point EnvironmentCoordMapper::getCoords(const Intersection& hit) const
{
    Vector I = (-hit.ray.d.normalize() + 2 * dot(hit.ray.d.normalize(), hit.normal().normalize()) * hit.normal().normalize()).normalize();
    Vector sn = I + hit.ray.d;
    float len = sn.length();
    Assert(len != 0.f) << " devision by zero in env mapper";
    return Point(sn.x / (2 * len) + 0.5, sn.y / (2 * len) + 0.5, 0);
}