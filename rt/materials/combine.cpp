#include <rt/materials/combine.h>
#include <core/color.h>
using namespace rt;

CombineMaterial::CombineMaterial() :sampling_mode_set(false){ SamplingMode = SAMPLING_NOT_NEEDED; }
void CombineMaterial::add(Material* material, float weight)
{
    materials.push_back(material);
    weights.push_back(weight);
    if (sampling_mode_set) { // update sampling mode if required
        if (SamplingMode != material->useSampling())
            SamplingMode = Sampling::SAMPLING_SECONDARY;
    }
    else { // set sampling mode to current material sampling mode
        SamplingMode = material->useSampling();
        sampling_mode_set = true;
    }
}

RGBColor CombineMaterial::getReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir, const Vector& inDir) const
{
    RGBColor color(0,0,0);
    for (int i = 0; i < materials.size(); ++i)
        color = color + weights[i] * materials[i]->getReflectance(texPoint, normal, outDir, inDir);
    return color;
}

RGBColor CombineMaterial::getEmission(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    RGBColor color(0,0,0);
    for (int i = 0; i < materials.size(); ++i)
        color = color + weights[i] * materials[i]->getEmission(texPoint, normal, outDir);
    return color;
}

Material::SampleReflectance CombineMaterial::getSampleReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir) const
{
    for (int i = 0; i < materials.size(); ++i)
        if (materials[i]->useSampling() == SAMPLING_ALL || materials[i]->useSampling() == SAMPLING_SECONDARY)
        {
            Material::SampleReflectance samp = materials[i]->getSampleReflectance(texPoint, normal, outDir) ;
            samp.direction = samp.direction * weights[i];
            //samp.reflectance = samp.reflectance * weights[i];
            return samp;
        }
    return Material::SampleReflectance();


     //Assuming that at most one of the combined materials requires sampling
    //Material::SampleReflectance ret = Material::SampleReflectance();
    //ret.direction = Vector::rep(0.0f);
    //bool set_dir = false;
    //if (SamplingMode != Sampling::SAMPLING_NOT_NEEDED) {
    //    for (int i = 0; i < this->materials.size(); i++) {
    //        Material::SampleReflectance m_reflectance = materials[i]->getSampleReflectance(texPoint, normal, outDir);
    //        ret.reflectance = ret.reflectance + m_reflectance.reflectance * this->weights[i];
    //        //            ret.direction = ret.direction + m_reflectance.direction * this->m_weights[i];

    //        if (materials[i]->useSampling() != Sampling::SAMPLING_NOT_NEEDED) {
    //            if (set_dir) {
    //                ret.direction = ret.direction + m_reflectance.direction * this->weights[i];
    //            }
    //            else {
    //                ret.direction = m_reflectance.direction * this->weights[i];
    //                set_dir = true;
    //            }
    //        }
    //    }
    //}
    //    ret.direction = ret.direction.normalize();
    //return ret;
}

Material::Sampling CombineMaterial::useSampling() const
{
    return SamplingMode;
}