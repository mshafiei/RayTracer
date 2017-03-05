#ifndef CG1RAYTRACER_MATERIALS_GLASS_HEADER
#define CG1RAYTRACER_MATERIALS_GLASS_HEADER

#include <rt/materials/material.h>
#include <core/random.h>

namespace rt {

class GlassMaterial : public Material {
public:
    GlassMaterial(float eta);
    virtual RGBColor getReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir, const Vector& inDir) const;
    virtual RGBColor getEmission(const Point& texPoint, const Vector& normal, const Vector& outDir) const;
    virtual SampleReflectance getSampleReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir) const;
    virtual Sampling useSampling() const { return Material::SAMPLING_ALL; }
    rt::Vector reflect(const Vector& normal,
                                      const Vector& incident) const;
    rt::Vector refract(const Vector& normal,
                                      const Vector& incident,
                                      float n1, float n2) const;
    float rSchlick2(const Vector& normal,
                                   const Vector& incident,
                                   float n1, float n2) const;
private:
    float eta;
};

}

#endif
