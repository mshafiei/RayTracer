#include <rt/lights/projectivelight.h>
#include <core/assert.h>
using namespace rt;
LightHit ProjectiveLight::getLightHit(const Point& p) const
{
    /*LightHit L;
    L.direction = Vector(position - p);
    L.distance = L.direction.length();
    L.direction = L.direction.normalize();
    return L;
*/
    NOT_IMPLEMENTED;
}

RGBColor ProjectiveLight::getIntensity(const LightHit& irr) const
{
 /*   Assert(irr.distance) << "Devision by zero PointLight";
    return (1 / (irr.distance*irr.distance)) * intensity;*/
    NOT_IMPLEMENTED;
}