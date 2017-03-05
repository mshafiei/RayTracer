#ifndef CG1RAYTRACER_INTEGRATORS_RECURSIVERAYTRACING_HEADER
#define CG1RAYTRACER_INTEGRATORS_RECURSIVERAYTRACING_HEADER

#include <rt/integrators/integrator.h>

namespace rt {

class World;
class Ray;
class RGBColor;

class RecursiveRayTracingIntegrator : public Integrator {
public:
    RecursiveRayTracingIntegrator(World* world);
    virtual RGBColor getRadiance(const Ray& ray) const;
private:
    mutable int depth;
    const int MaxDepth;
};

}

#endif
