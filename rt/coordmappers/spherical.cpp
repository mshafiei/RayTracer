#include "rt/coordmappers/spherical.h"
#include "core/assert.h"
#include "rt/intersection.h"

using namespace rt;

SphericalCoordMapper::SphericalCoordMapper()
{

}

SphericalCoordMapper::SphericalCoordMapper(const Point& origin, const Vector& zenith, const Vector& azimuthRef) :
origin(origin), zenith(zenith), azimuthRef(azimuthRef)
{

}

Point SphericalCoordMapper::getCoords(const Intersection& hit) const
{ 
  
    Vector vHit = hit.hitPoint() - origin;
    Vector normal = vHit.normalize();
    // project vHit on the xz plane (here we use the convention
    // that the equator of the sphere is in the xz plane since y
    // is the up vector)
    Vector up = zenith.normalize();
    Vector xzProj = (vHit - dot(vHit, up) * up).normalize();
    float theta = acosf(dot(normal, up));
    float phi = atan2f(xzProj[1], xzProj[0]);
    if (phi < 0) phi += 2.f*pi;
    float u = phi / (2 * pi*azimuthRef.length());
    float v = theta / (pi*zenith.length());
    return Point(u, v, 0);
}