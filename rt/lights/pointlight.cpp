#include <rt/lights/pointlight.h>
#include <core/assert.h>
using namespace rt;

PointLight::PointLight(const Point& position, const RGBColor& intensity) :position(position), intensity(intensity)
{

}

LightHit PointLight::getLightHit(const Point& p) const
{
    LightHit L;
    L.direction = Vector(position - p);
    L.distance = L.direction.length();
    L.direction = L.direction.normalize();
    return L;

}

RGBColor PointLight::getIntensity(const LightHit& irr) const
{
    Assert(irr.distance) << "Devision by zero PointLight";
    return (1 / (irr.distance*irr.distance)) * intensity;
}
