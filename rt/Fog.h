#pragma once

#include <core/color.h>
namespace rt
{
    class Fog
    {
    public:
        Fog(float OptDensity, const RGBColor& VolumeRadiance) :VolumeRadiance(VolumeRadiance),OptDensity(OptDensity){}
        RGBColor getVolRadiance(){ return VolumeRadiance; }
        float getTransmittance(float dist)
        {
            return exp(-OptDensity*dist);
        }
    private:
        float OptDensity;
        RGBColor VolumeRadiance;
    };
}