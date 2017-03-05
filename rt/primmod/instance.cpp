
#include <rt/primmod/instance.h>
#include <core/util.h>
#include <rt/bbox.h>
#include <rt/intersection.h>
#include <iostream>

namespace rt
{
    
    Instance::Instance(Primitive* content) :ptrContent(content)
    {
        reset();

    }
    void Instance::reset()
    {
        trans = Matrix::identity();
        computeTransBounds();
        inverseTrans = trans.invert();
        inverseTransPose = inverseTrans.transpose();
    }

    void Instance::translate(const Vector& t)
    {
        Matrix transMat = Matrix::identity();
        
        transMat[0][3] = t.x;
        transMat[1][3] = t.y;
        transMat[2][3] = t.z;

        trans = product(transMat, trans);
        computeTransBounds();
        inverseTrans = trans.invert();
        inverseTransPose = inverseTrans.transpose();
    }
    
    void Instance::rotate(const Vector& axis, float angle)
    {
        Vector a = axis.normalize();
        float s = sinf(angle);
        float c = cosf(angle);
        
        trans[0][0] = a.x * a.x + (1.f - a.x * a.x) * c;
        trans[0][1] = a.x * a.y * (1.f - c) - a.z * s;
        trans[0][2] = a.x * a.z * (1.f - c) + a.y * s;
        
        trans[1][0] = a.x * a.y * (1.f - c) + a.z * s;
        trans[1][1] = a.y * a.y + (1.f - a.y * a.y) * c;
        trans[1][2] = a.y * a.z * (1.f - c) - a.x * s;
        
        trans[2][0] = a.x * a.z * (1.f - c) - a.y * s;
        trans[2][1] = a.y * a.z * (1.f - c) + a.x * s;
        trans[2][2] = a.z * a.z + (1.f - a.z * a.z) * c;
        
        trans[3][3] = 1;
        computeTransBounds();
        inverseTrans = trans.invert();
        inverseTransPose = inverseTrans.transpose();
    }

    void Instance::scale(float scale)
    {
        Matrix scalMat = Matrix::identity();
        scalMat[0][0] = scale;
        scalMat[1][1] = scale;
        scalMat[2][2] = scale;

        trans = product(scalMat, trans);        
        computeTransBounds();
        inverseTrans = trans.invert();
        inverseTransPose = inverseTrans.transpose();
    }

    void Instance::scale(const Vector& scale)
    {
        Matrix scalMat = Matrix::identity();
        //std::cout << scale.y << std::endl;
        scalMat[0][0] = scale.x;
        scalMat[1][1] = scale.y;
        scalMat[2][2] = scale.z;

        trans = product(scalMat, trans);
        computeTransBounds();
        inverseTrans = trans.invert();
        inverseTransPose = inverseTrans.transpose();
    }

    BBox Instance::getBounds() const
    {
        return transformedBounds;
    }

    void Instance::computeTransBounds()
    {
        BBox bounds = ptrContent->getBounds();
        transformedBounds = BBox::empty();
        Point origPs[8];
        Point transPs[8];
        //building 8 vertices of the bounding box
        for (int i = 0; i < 8; ++i)
            origPs[i] = bounds.min;

        origPs[1].x = bounds.max.x;
        origPs[2].y = bounds.max.y;
        origPs[3].z = bounds.max.z;
        origPs[4].x = bounds.max.x; origPs[4].y = bounds.max.y;
        origPs[5].x = bounds.max.x; origPs[5].z = bounds.max.z;
        origPs[6].y = bounds.max.y; origPs[6].z = bounds.max.z;
        origPs[7] = bounds.max;

        for (int i = 0; i < 8; ++i)
        {
            Point p = trans * origPs[i];
            transformedBounds.extend(p);
        }
    }
    Intersection Instance::intersect(const Ray& ray, float previousBestDistance) const
    {
        Intersection inters;
        Ray inverseRay;
        float inversePreBestDist;
        
        inverseRay.d = (inverseTrans * ray.d).normalize();
        inverseRay.o = inverseTrans * ray.o;

        inters = ptrContent->intersect(inverseRay, FLT_MAX);

        if (previousBestDistance == FLT_MAX)
            inversePreBestDist = previousBestDistance;
        else if (inters)
        {
            Point inverseP = trans * inters.ray.getPoint(inters.distance);
            float dist = (inverseP - ray.o).length();
            if (dist > previousBestDistance)
                inters = Intersection::failure();
        }

        Intersection interse = Intersection::failure();
        if (inters)
        {
            Point normalHitP = trans * inters.hitPoint();
            interse = Intersection((normalHitP - ray.o).length(), ray, inters.solid, (inverseTransPose * inters.normal()).normalize(), inters.local());
        }
        return interse;
    }
}
