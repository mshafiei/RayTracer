#include <rt/integrators/casting.h>
#include <core/color.h>
#include <rt/intersection.h>
#include <rt/world.h>
#include <iostream>
#include <cmath>

namespace rt
{
    RGBColor RayCastingIntegrator::getRadiance(const Ray& ray) const
    {
        Intersection hit = world->scene->intersect(ray);

        if(hit)
        {
            float cos = fabs(dot(ray.d, hit.normal()));
            return RGBColor(cos, cos, cos);
        }
        else
        {
            //return RGBColor(0.5f, 0.5f, 0.5f);
            return RGBColor(0.0f, 0.0f, 0.0f);
        }
    }
}
