#include <rt/lights/arealight.h>

namespace rt {
AreaLight::AreaLight(Solid* solid) : solid(solid)
{}

LightHit AreaLight::getLightHit(const Point& p) const
{
    Point position = solid->sample();
    LightHit L;
    L.direction = Vector(position - p);
    L.distance = L.direction.length();
    L.direction = L.direction.normalize();
    L.solid = solid;
    return L;
}

RGBColor AreaLight::getIntensity(const LightHit& irr) const
{
    float area = solid->getArea();
    Assert(irr.distance) << "Devision by zero PointLight";

    Point sample_pt = Point::rep(0.0f);
    Vector normDummy = Vector(0, 0, 1);
    Vector outDummy = Vector(0, 0, -1);

    RGBColor intensity = area * (1 / (irr.distance*irr.distance))
        * solid->material->getEmission(sample_pt, normDummy,
        outDummy);
    return intensity;
}
};
