#include <rt/materials/phong.h>
#include <core/assert.h>
#include <core/scalar.h>
using namespace rt;
PhongMaterial::PhongMaterial(Texture* specular, float exponent) :specular(specular), exponent(exponent)
{

}

RGBColor PhongMaterial::getReflectance(const Point& texPoint,
                                       const Vector& normal,
                                       const Vector& outDir,
                                       const Vector& inDir) const
{
    float dot = rt::dot(normal, inDir);
    Vector reflection = (normal * (dot*2)) - inDir;
    reflection = reflection.normalize();
    float specWeight = rt::dot(reflection, outDir);

    specWeight = (specWeight < 0.0f) ? 0.0f : powf(specWeight,
                                                  exponent);
    RGBColor specCoef = specular->getColor(texPoint) * specWeight;

    return (specCoef * dot * rt::pi / 2.0f);
}

RGBColor PhongMaterial::getEmission(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    return RGBColor::rep(0);// specular->getColor(texPoint);
}

Material::SampleReflectance PhongMaterial::getSampleReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    /*
    float dot = rt::dot(normal, outDir);
    Vector reflection = (normal * (dot*2)) - outDir;
    reflection = reflection.normalize();
    RGBColor ref = getReflectance(texPoint, normal, outDir,
                                  reflection);
    return SampleReflectance(reflection, ref);
    */
    return SampleReflectance();
}

Material::Sampling PhongMaterial::useSampling() const
{
    return Material::SAMPLING_NOT_NEEDED;
}
