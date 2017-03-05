#ifndef CG1RAYTRACER_LIGHTS_AREALIGHT_HEADER
#define CG1RAYTRACER_LIGHTS_AREALIGHT_HEADER

#include <core/vector.h>
#include <rt/lights/light.h>
#include <rt/solids/solid.h>
#include <core/point.h>
#include <core/color.h>
#include <core/assert.h>
#include <rt/materials/material.h>
#include <rt/solids/quad.h>

namespace rt {

class AreaLight : public Light {
public:
    AreaLight() {}
    AreaLight(Solid* source);
    virtual LightHit getLightHit(const Point& p) const;
    virtual RGBColor getIntensity(const LightHit& irr) const;

private:
    Solid* solid;
};

}

#endif

