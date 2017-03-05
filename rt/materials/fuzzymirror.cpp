#include <rt/materials/fuzzymirror.h>

namespace rt {
    FuzzyMirrorMaterial::FuzzyMirrorMaterial(float eta, float kappa,
                                             float fuzzyangle)
            : eta(eta), kappa(kappa), fuzzyangle(fuzzyangle)
    {

    }

    RGBColor FuzzyMirrorMaterial::getReflectance(const Point& texPoint,
                                                 const Vector& normal,
                                                 const Vector& outDir,
                                                 const Vector& inDir) const
    {
        return RGBColor::rep(1.0f);
    }

    RGBColor FuzzyMirrorMaterial::getEmission(const Point& texPoint,
                                              const Vector& normal,
                                              const Vector& outDir) const
    {
        return RGBColor::rep(0);
    }

    Material::SampleReflectance FuzzyMirrorMaterial::getSampleReflectance(
        const Point& texPoint,
        const Vector& normal,
        const Vector& outDir) const
    {
        float dot = rt::dot(outDir, normal);
        Vector ref = (-outDir +
                      2 * dot * normal).normalize();
        
        // r (of the imaginary disk) = tan(fuzzyangle)
        float r = std::tan(fuzzyangle * rt::random());
        // choose a random angle in the disk
        float theta = rt::random() * 360.0;
        // find a vector perpendicular to the reflection direction
        // (here we choose the component of normal perpendicular to
        // reflection)
        Vector perp = (normal - (dot * ref)).normalize();
        perp = (Rotate(theta, ref))(perp) * r;
        ref = (ref + perp).normalize();
        
        return Material::SampleReflectance(ref,
                                           getReflectance(texPoint, normal, outDir, ref));
    }
};
