#include <rt/integrators/homvolintegrator.h>
#include <rt/intersection.h>
#include <rt/world.h>
#include <rt/lights/light.h>
#include <rt/coordmappers/coordmapper.h>
#include <rt/materials/material.h>
#include <core/color.h>
using namespace rt;
RGBColor HomogenousVolumeIntegrator::getRadiance(const Ray& ray) const
{
    //TODO: what if there is a glassy object
    Intersection hit = world->scene->intersect(ray);
    RGBColor color(0, 0, 0);
    if (hit)
    {
        Point localHitPoint = hit.solid->texMapper->getCoords(hit);
        
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
                if ((obstacle.solid == hit.solid || obstacle.distance >= lHit.distance))
                {

                    RGBColor tmp;
                    if(lHit.distance == 0)
                        tmp = hit.solid->material->getReflectance(localHitPoint, -hit.ray.d, -hit.ray.d, -lHit.direction);
                    else
                       tmp = hit.solid->material->getReflectance(localHitPoint, hit.normal(), -hit.ray.d, -lHit.direction);
                    
                    if (world->fog)
                    {
                        float att = world->fog->getTransmittance(lHit.distance);
                        tmp = tmp * att;
                    }
                    color = color + tmp * (*p)->getIntensity(lHit);
                }
            }
        }
        color = color + hit.solid->material->getEmission(localHitPoint, hit.normal(), -hit.ray.d);
        if (world->fog)
        {
            float att = world->fog->getTransmittance(hit.ray.t);
            color = (1 - att) * world->fog->getVolRadiance() + att * color;
        }

        color = color.clamp();
    }
    return color;
}