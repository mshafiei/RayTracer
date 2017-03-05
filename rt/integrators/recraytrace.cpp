#include <rt/integrators/recraytrace.h>
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

RecursiveRayTracingIntegrator::RecursiveRayTracingIntegrator(World* world) : Integrator(world), depth(0), MaxDepth(6)
{

}

RGBColor RecursiveRayTracingIntegrator::getRadiance(const Ray& ray) const
{
    
    Intersection hit = world->scene->intersect(ray);
    RGBColor color(0, 0, 0);
    if (hit)
    {
        Point hit_point = hit.solid->texMapper->getCoords(hit);
        if (depth < MaxDepth && 
            (hit.solid->material->useSampling() == Material::SAMPLING_ALL||
             hit.solid->material->useSampling() == Material::SAMPLING_SECONDARY))
        { 
            depth++;
            Vector hit_normal = hit.normal();
            Vector out_dir = -hit.ray.d;
            Material::SampleReflectance sample =
                hit.solid->material->getSampleReflectance(
                hit_point, hit_normal, out_dir);

            if(sample.refractive)
            {
                Ray ray = Ray(hit_point
                              + (hit_normal * 0.0001f
                                 * (sample.inside ? -1.0f : 1.0f)),
                              sample.direction);
                float cosAngle = std::abs(rt::dot(hit_normal,
                                                  sample.direction));
                color = color + (sample.reflection_pr
                                 * sample.reflectance *
                                 getRadiance(ray) * cosAngle);

                if(sample.refraction_pr > 0.0f)
                {
                    ray = Ray(hit_point
                                  + (hit_normal * 0.0001f
                                     * (sample.inside ? 1.0f : -1.0f)),
                                  sample.refraction_dir);
                    cosAngle = std::abs(
                        rt::dot(hit_normal, sample.refraction_dir));
                    color = color + (sample.refraction_pr
                                     * sample.refraction_reflectance
                                     * getRadiance(ray) * cosAngle);
                }
            }
            else
            {
                Ray ray = Ray(hit_point + hit_normal*0.0001f,
                              sample.direction);
                float cosAngle = std::abs(rt::dot(hit_normal,
                                                  sample.direction));
                color = color + sample.reflectance * getRadiance(ray) * cosAngle;                
            }
            
            depth--;
        }

        if (hit.solid->material->useSampling() == Material::SAMPLING_NOT_NEEDED ||
            hit.solid->material->useSampling() == Material::SAMPLING_SECONDARY)
        {
            for (auto p = world->light.begin(); p != world->light.end(); ++p)
            {
                Vector hit_normal = hit.normal();
                LightHit lHit = (*p)->getLightHit(hit_point);
                float d1 = dot(lHit.direction, hit_normal);
                float d2 = dot(ray.d, hit_normal);
                if ((d1 >= 0 && d2 <= 0) || (d1 <= 0 && d2 >= 0))
                {
                    Ray shdwray(hit_point, lHit.direction);
                    shdwray.o = shdwray.getPoint(0.0001f);
                    Intersection obstacle = world->scene->intersect(shdwray);
                    if (obstacle.solid == lHit.solid || obstacle.distance >= lHit.distance)
                    {

                        // inDir is treated as the dir from the point
                        // to the light source, outDir as the dir from
                        // the point to the eye
                        RGBColor intensity = (*p)->getIntensity(lHit);
                        RGBColor reflectance = hit.solid->material->getReflectance(hit_point, hit.normal(), -hit.ray.d, lHit.direction);
                        color = color + (intensity * reflectance);
                    }
                }
            }
            color = color +
                hit.solid->material->getEmission(
                hit_point, hit.normal(),
                -hit.ray.d);
            
        }
    }
    return color.clamp();
}
