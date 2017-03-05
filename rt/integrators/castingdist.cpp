#include <rt/integrators/castingdist.h>
#include <core/color.h>
#include <rt/intersection.h>
#include <rt/world.h>
#include <iostream>
#include <cmath>

namespace rt
{

    float clamp(float min, float max, float f);
    
    RayCastingDistIntegrator::RayCastingDistIntegrator(World* world,
                                                       const RGBColor& nearColor,
                                                       float nearDist,
                                                       const RGBColor& farColor,
                                                       float farDist)
            : Integrator(world), nearColor(nearColor),
              nearDist(nearDist), farColor(farColor), farDist(farDist)
    {        
    }

    RGBColor RayCastingDistIntegrator::getRadiance(const Ray& ray) const
    {
        Intersection hit = world->scene->intersect(ray);

        if(hit)
        {
            float distance = clamp(nearDist, farDist, hit.distance);
            distance = (distance - nearDist) / (farDist - nearDist);
            float cos = fabs(dot(ray.d, hit.normal()));
            RGBColor c = interp(nearColor, farColor, distance) * cos;
            return c.clamp();
        }
        else
        {
            return RGBColor(0, 0, 0);
        }
    }

    float clamp(float min, float max, float f)
    {
        if(f < min)
        {
            return min;
        }
        else if(f > max)
        {
            return max;
        }

        return f;
    }
}
