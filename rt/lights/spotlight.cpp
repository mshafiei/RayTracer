#include <rt/lights/spotlight.h>
#include <core/assert.h>

#include <math.h>

using namespace rt;

SpotLight::SpotLight(const Point& position, const Vector& direction, float angle, float exp, const RGBColor& intensity):
position(position), direction(direction), exp(exp), intensity(intensity)
{
    cosAngle = cos(angle);
}

LightHit SpotLight::getLightHit(const Point& p) const
{
    LightHit L;
    L.direction = Vector(position - p);
    L.distance = L.direction.length();
    L.direction = L.direction.normalize();
    return L;
}

RGBColor SpotLight::getIntensity(const LightHit& irr) const
{
    float innerCos = dot(-irr.direction.normalize(), direction);
    Assert(irr.distance) << "Devision by zero in spotlight";
    RGBColor color(0, 0, 0);
    if (innerCos > cosAngle)
        color = intensity * powf(innerCos,exp)* (1.0f / (irr.distance*irr.distance));
    return color;
}
