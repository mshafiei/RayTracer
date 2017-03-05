#include <rt/materials/lambertian.h>
#include <rt/materials/material.h>
#include <core/color.h>
#include <core/assert.h>
#include <core/scalar.h>
using namespace rt;
LambertianMaterial::LambertianMaterial(Texture* emission, Texture* diffuse) :emission(emission), diffuse(diffuse)
{

}

// outDir as the dir from the point to the eye
// inDir is treated as the dir from the point to the light source
RGBColor LambertianMaterial::getReflectance(const Point& texPoint,
                                            const Vector& normal,
                                            const Vector& outDir,
                                            const Vector& inDir) const
{
    float dot = fabs(rt::dot(normal, inDir));
    RGBColor diffuseCoef = diffuse->getColor(texPoint) / rt::pi;
    //return RGBColor(1, 1, 1);
    return (diffuseCoef * dot);
}

RGBColor LambertianMaterial::getEmission(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    //return RGBColor(1,1,1);
    return emission->getColor(texPoint);
}

Material::SampleReflectance LambertianMaterial::getSampleReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    return SampleReflectance();
}

Material::Sampling LambertianMaterial::useSampling() const
{
    return Material::SAMPLING_NOT_NEEDED;
}
