#include "rt/coordmappers/cylindrical.h"
#include "core/assert.h"
#include "rt/intersection.h"
using namespace rt;
CylindricalCoordMapper::CylindricalCoordMapper(const Point& origin, const Vector& longitudinalAxis, const Vector& polarAxis):
origin(origin), longitudinalAxis(longitudinalAxis), polarAxis(polarAxis)
{ 

}

Point CylindricalCoordMapper::getCoords(const Intersection& hit) const
{ 
    Vector vHit = hit.hitPoint() - origin;
    Vector normal = vHit.normalize();

    Vector up = longitudinalAxis.normalize();
    float v = dot(vHit, up)/longitudinalAxis.length();
    Vector xzProj = (vHit - dot(vHit, up) * up).normalize();
    float phi = atan2f(xzProj[1], xzProj[0]);
    if (phi < 0)phi += 2.f*pi;
    float u = phi / (2 * pi *polarAxis.length());
    return Point(u, v, 0);
}