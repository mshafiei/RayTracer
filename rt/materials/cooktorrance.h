//#pragma once
//
//#include <rt/materials/material.h>
//#include <rt/textures/texture.h>
//
//namespace rt {
//
//    class CookTorranceMaterial : public Material {
//    public:
//        CookTorranceMaterial(float kd, float fresnelMax, float roughness);
//        virtual RGBColor getReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir, const Vector& inDir) const;
//        virtual RGBColor getEmission(const Point& texPoint, const Vector& normal, const Vector& outDir) const;
//        virtual SampleReflectance getSampleReflectance(const Point& texPoint, const Vector& normal, const Vector& outDir) const;
//        virtual Sampling useSampling() const;
//    
//private:
//    float kd;
//    float fresnelMax;
//    float roughness;
//    };
//}
