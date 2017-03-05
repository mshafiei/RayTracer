#include <rt/lights/Ambient.h>

using namespace rt;

LightHit AmbientLight::getLightHit(const Point& p) const
{
    LightHit l;
    l.distance = 0;
    l.direction = Vector(0, 0, 0);
    return l;
}

RGBColor AmbientLight::getIntensity(const LightHit& irr) const
{
    return intensity;
}