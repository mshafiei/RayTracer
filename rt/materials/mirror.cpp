#include <rt/materials/mirror.h>
#include <core/assert.h>
#include <core/scalar.h>
using namespace rt;
MirrorMaterial::MirrorMaterial(float eta, float kappa) :eta(eta), kappa(kappa)
{

}

RGBColor MirrorMaterial::getReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
    Vector m = 2 * (dot(normal, outDir))*normal - outDir;
    if (inDir == m)
        return RGBColor::rep(1.0f);
    else
        return RGBColor::rep(0.0f);
}

RGBColor MirrorMaterial::getEmission(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    return RGBColor::rep(0);
}

Material::SampleReflectance MirrorMaterial::getSampleReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    Vector norm_normalize, out_normalize;
    norm_normalize = normal.normalize();
    out_normalize = outDir.normalize();

    Vector I = (-out_normalize + 2 * dot(out_normalize, norm_normalize) * norm_normalize).normalize();
    //RGBColor::rep(1 - kappa)
    return Material::SampleReflectance(I, getReflectance(texPoint, normal, outDir, I));
}

Material::Sampling MirrorMaterial::useSampling() const
{
    return Material::SAMPLING_ALL;
}