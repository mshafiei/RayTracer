#include <rt/integrators/raytrace.h>
#include <rt/world.h>
#include <rt/intersection.h>
#include <core/color.h>
#include <rt/lights/pointlight.h>
#include <core/util.h>
#include <core/point.h>
#include <rt/materials/material.h>
#include <rt/solids/solid.h>
#include <rt/coordmappers/coordmapper.h>
#include <rt/solids/specialsolid.h>
using namespace rt;

RGBColor RayTracingIntegrator::getRadiance(const Ray& ray) const
{
    //TODO: what if there is a glassy object
    Intersection hit = world->scene->intersect(ray);
    RGBColor color(0, 0, 0);
    if (hit)
    {
        Point localHitPoint = hit.solid->texMapper->getCoords(hit);
        if (world->light.size() == 0)
            color = hit.solid->material->getEmission(hit.hitPoint(), Vector(), Vector());
        else
        {
            for (auto p = world->light.begin(); p != world->light.end(); ++p)
            {
                LightHit lHit = (*p)->getLightHit(hit.hitPoint());
                float d1 = dot(lHit.direction, hit.normal());
                float d2 = dot(ray.d, hit.normal());
                if ((d1 >= 0 && d2 <= 0) || (d1 <= 0 && d2 >= 0))
                {
                    Ray shdwray(hit.hitPoint(), lHit.direction);
                    shdwray.o = shdwray.getPoint(0.0001f);
                    Intersection obstacle = world->scene->intersect(shdwray);
                    if (typeid(obstacle.solid) != typeid(SpecialSolid) && (obstacle.solid == hit.solid || obstacle.distance >= lHit.distance))
                    {
                        RGBColor tmp = hit.solid->material->getReflectance(localHitPoint, hit.normal(), -hit.ray.d, -lHit.direction);
                        //if (tmp.r > 0 && tmp.g > 0 && tmp.b > 0)
                        color = color + tmp * (*p)->getIntensity(lHit);
                    }
                }
            }
            color = color + hit.solid->material->getEmission(localHitPoint, hit.normal(), -hit.ray.d);
        }
        color = color.clamp();
    }
    return color;
}