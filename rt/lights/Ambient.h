#pragma once

#include <rt/lights/light.h>
#include <core/color.h>
using namespace rt;

class AmbientLight :public Light
{
public:
    AmbientLight(const RGBColor& intensity) :intensity(intensity){}
    virtual LightHit getLightHit(const Point& p) const;
    virtual RGBColor getIntensity(const LightHit& irr) const;

private:
    RGBColor intensity;
};