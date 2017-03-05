#include <rt/materials/glass.h>
#include <core/assert.h>
namespace rt {
GlassMaterial::GlassMaterial(float eta) : eta(eta)
{}

RGBColor GlassMaterial::getReflectance(const Point& texPoint,
                                       const Vector& normal,
                                       const Vector& outDir,
                                       const Vector& inDir) const
{
    return RGBColor(1, 1, 1);
}

RGBColor GlassMaterial::getEmission(const Point& texPoint,
                                    const Vector& normal,
                                    const Vector& outDir) const
{
    return RGBColor(0, 0, 0);
}

    // helper method to calculate reflection direction
    rt::Vector GlassMaterial::reflect(const Vector& normal,
                                      const Vector& incident) const
    {
        const float cosI = -rt::dot(normal.normalize(), incident.normalize());
        return incident + (2 * cosI * normal);
    }
    
    // helper method to calculate refraction direction
    rt::Vector GlassMaterial::refract(const Vector& normal,
                                      const Vector& incident,
                                      float n1, float n2) const
    {
        const float n = n1 / n2;
        //const float cosI = -rt::dot(normal.normalize(), incident.normalize());
        //const float sinT2 = n * n * (1.0f - cosI * cosI);
        //if(sinT2 > 1.0f) return rt::Vector(0, 0, 0);
        //const float cosT = std::sqrtf(1.0f - sinT2);
        //return n * incident + (n * cosI - cosT) * normal;

        const float cosI = rt::dot(normal.normalize(), incident.normalize());
        const float sinT2 = n * n * (1.0f - cosI * cosI);
        if(sinT2 > 1.0f) return rt::Vector(0, 0, 0);
        const float cosT = std::sqrt(1.0f - sinT2);
        return n * incident - (n + cosT) * normal;

    }

    // helper method to calculate refraction probablity using Schlik's
    // approximation of Fresnell's euqation
    float GlassMaterial::rSchlick2(const Vector& normal,
                                   const Vector& incident,
                                   float n1, float n2) const
    {
        float r0 = (n1 - n2) / (n1 + n2);
        r0 *= r0;
        float cosX = -dot(normal.normalize(), incident.normalize());

        if(n1 > n2)
        {
            const float n = n1 / n2;
            const float sinT2 = n * n * (1.0f - cosX * cosX);
            // in case of total internal reflection
            // always relfect don't refract
            if(sinT2 > 1.0f) return 1.0f;
            cosX = std::sqrt(1.0f - sinT2);
        }

        const float x = 1.0f - cosX;
        return r0 + (1.0f - r0) * x * x * x * x * x;
    }
    
// http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
    Material::SampleReflectance GlassMaterial::getSampleReflectance(
        const Point& texPoint, const Vector& normal,
        const Vector& outDir) const
    {
        SampleReflectance result;
        Vector outDirNorm = -outDir.normalize();
        float dot = rt::dot(normal, outDirNorm);

        result.inside = (dot > 0.0f);
        result.refractive = true;

        float n1, n2;
        n1 = result.inside ? eta : 1;
        n2 = result.inside ? 1 : eta;

        result.reflection_pr = rSchlick2(normal, outDirNorm, n1, n2);
        result.direction = reflect(normal, outDirNorm);
        result.reflectance = getReflectance(texPoint, normal, outDir,
            result.direction);

        result.refraction_pr = 1 - result.reflection_pr;
        result.refraction_dir = refract(normal, outDirNorm, n1, n2);
        result.refraction_reflectance = getReflectance(
            texPoint, normal, outDir, result.refraction_dir);

        return result;
    }

    /*
Material::SampleReflectance GlassMaterial::getSampleReflectance(
    const Point& texPoint, const Vector& normal,
    const Vector& outDir) const
{
    Vector outDirNorm = -outDir.normalize();
    float dot = rt::dot(normal, outDirNorm);
    bool inside = (dot > 0.0f);
    // air index of refraction;
    float n1 = 1;
    // glass index of refraction
    float n2 = eta;

    // if inside, reverse index of refractions
    if(inside)
    {
        n1 = eta;
        n2 = 1;
    }

    // calculate Schlick’s approximation
    float r0 = ((n1 - n2) * (n1 - n2)) / ((n1 + n2) * (n1 + n2));
    float r = r0 + (1.0f - r0) * std::pow(1 - std::abs(dot), 5.0f);

    float reflectionPr = r;
    Vector reflectionDir = outDirNorm - (normal * (dot*2));
    float factor = n1 / n2;
    float sinRefractionSqrd = factor * factor * (1.0f - dot*dot);

    float refractionPr;
    Vector refractionDir;
    
    // total internal reflection
    if(sinRefractionSqrd > 1.0)
    {
        refractionPr = 0.0f;
        refractionDir = Vector(0.0f, 0.0f, 0.0f);
    }
    else
    {
        refractionPr = 1.0f - r;
        float cosRefraction = std::sqrt(1.0f - sinRefractionSqrd);
        float scale;
        Vector newNormal;
        if(inside)
        {
            scale = factor * dot - cosRefraction;
            newNormal = normal;
        }
        else
        {
            scale = factor * dot + cosRefraction;
            newNormal = -normal;
        }

        refractionDir = ((scale * newNormal)
                         + (factor * outDirNorm)).normalize();
    }

    float rand = rt::random();
    SampleReflectance result;

    result.inside = inside;
    result.refractive = true;

    result.reflection_pr = 1 - refractionPr;
    result.direction = reflectionDir;
    result.reflectance = getReflectance(texPoint, normal, outDir,
                                        reflectionDir);

    result.refraction_pr = refractionPr;
    result.refraction_dir = refractionDir;
    result.refraction_reflectance = getReflectance(texPoint, normal,
                                                   outDir,
                                                   refractionDir);

    return result;
}
*/
};
