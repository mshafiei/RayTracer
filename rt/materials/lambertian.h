#ifndef CG1RAYTRACER_MATERIALS_LAMBERTIAN_HEADER
#define CG1RAYTRACER_MATERIALS_LAMBERTIAN_HEADER

#include <rt/materials/material.h>
#include <rt/textures/texture.h>
#include <core/point.h>
#include <core/vector.h>

namespace rt {

class LambertianMaterial : public Material {
public:
    LambertianMaterial(Texture* emission, Texture* diffuse);
    virtual RGBColor getReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir, const Vector& inDir) const;
    virtual RGBColor getEmission(const Point& texPoint, const Vector& normal, const Vector& outDir) const;
    virtual SampleReflectance getSampleReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir) const;
    virtual Sampling useSampling() const;
private:
    Texture* emission;
    Texture* diffuse;
};

}

#endif
