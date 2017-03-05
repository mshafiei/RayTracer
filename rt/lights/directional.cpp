#include <rt/lights/directional.h>
#include <core/assert.h>
using namespace rt;
DirectionalLight::DirectionalLight(const Vector& direction, const RGBColor& color):direction(direction),color(color){}
LightHit DirectionalLight::getLightHit(const Point& p) const
{
    LightHit L;
    L.direction = -direction;
    L.distance = FLT_MAX;
    L.direction = L.direction.normalize();
    return L;
}
RGBColor DirectionalLight::getIntensity(const LightHit& irr) const
{
    return color;
}
