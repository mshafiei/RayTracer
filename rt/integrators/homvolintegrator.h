#include <rt/integrators/integrator.h>

namespace rt {
    class World;
    class Ray;
    class RGBColor;

    class HomogenousVolumeIntegrator : public Integrator {
    public:
        HomogenousVolumeIntegrator(World* world) : Integrator(world) {}
        virtual RGBColor getRadiance(const Ray& ray) const;
    };

}