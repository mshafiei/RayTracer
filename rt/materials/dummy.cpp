/*
* dummy.cpp
*
*  Created on: Dec 4, 2015
*      Author: sumit
*/

#include "dummy.h"
#include "material.h"
#include <core/assert.h>

using namespace rt; /* namespace rt */

rt::DummyMaterial::DummyMaterial()
{
    this->reflectance = 1;
}

RGBColor rt::DummyMaterial::getReflectance(const Point& texPoint, const Vector& normal,
    const Vector& outDir, const Vector& inDir) const
{
    Vector norm_normalize, in_normalize, out_normalize;
    norm_normalize = normal.normalize();
    in_normalize = inDir.normalize();


    return RGBColor::rep(reflectance * fabs(dot(in_normalize, norm_normalize)));
}
RGBColor rt::DummyMaterial::getEmission(const Point& texPoint, const Vector& normal,
    const Vector& outDir) const
{
    return RGBColor::rep(0);
}

rt::Material::SampleReflectance rt::DummyMaterial::getSampleReflectance(const Point& texPoint,
    const Vector& normal, const Vector& outDir) const
{
    NOT_IMPLEMENTED;
}

