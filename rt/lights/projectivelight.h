#pragma once
#include <rt/lights/light.h>
#include <core/point.h>
#include <core/color.h>
namespace rt
{
    class ProjectiveLight : public Light
    {
    public:
        ProjectiveLight();// : position(position){}

        LightHit getLightHit(const Point& p) const;

        RGBColor getIntensity(const LightHit& irr) const;
    private:
        //Point position;
    };
}