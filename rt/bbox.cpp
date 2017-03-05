#include <rt/bbox.h>
#include <rt/intersection.h>
#include <rt/solids/aabox.h>
namespace rt
{
    BBox::BBox(const Point& min, const Point& max) : min(min), max(max)
    { 
    }
    //empty box should intersect with no ray in the space
    BBox BBox::empty()
    { 
        BBox box;
        box.min = Point(FLT_MAX, FLT_MAX, FLT_MAX);
        box.max = Point(FLT_MIN, FLT_MIN, FLT_MIN);
        return box;
    }
    
    //full box contains all space and must intersect with every ray in the space
    //TODO: double check
    BBox BBox::full()
    {
        BBox box;
        box.min = Point(FLT_MIN, FLT_MIN, FLT_MIN);
        box.max = Point(FLT_MAX, FLT_MAX, FLT_MAX);
        return box;
    }

    std::pair<float, float> BBox::intersect(const Ray& ray) const
    {
        if (*this == empty())
        {
            //t1 > t2 means ray missed the box (according to sheet3)
            std::pair<float, float> p;
            p.first = FLT_MAX;
            p.second = FLT_MIN;
            return p;
        }
        Vector normals[3] =
        {
            Vector(-1, 0, 0),
            Vector(0, -1, 0),
            Vector(0, 0, -1)
        };

        float t0 = 0;
        float t1 = FLT_MAX;
        Vector intersectionNormal;

        for (int i = 0; i < 3; i++)
        {
            int swapped = 1;
            float invRayDir = 1.f / ray.d[i];
            float tNear = (min[i] - ray.o[i]) * invRayDir;
            float tFar = (max[i] - ray.o[i]) * invRayDir;

            if (tNear > tFar)
            {
                std::swap(tNear, tFar);
                swapped = -1;
            }

            if (tNear > t0)
            {
                t0 = tNear;
                intersectionNormal = swapped * normals[i];
            }

            t1 = tFar < t1 ? tFar : t1;

            if (t0 > t1)
            {
                return std::pair<float, float>(FLT_MAX, FLT_MIN);
            }
        }

        if (FLT_MAX > t0)
        {
            return std::pair<float, float>(t0, t1);
        }

        return std::pair<float, float>(FLT_MAX, FLT_MIN);
    }

    void BBox::extend(const Point& point)
    {
        min.x = point.x < min.x ? point.x : min.x;
        min.y = point.y < min.y ? point.y : min.y;
        min.z = point.z < min.z ? point.z : min.z;

        max.x = point.x > max.x ? point.x : max.x;
        max.y = point.y > max.y ? point.y : max.y;
        max.z = point.z > max.z ? point.z : max.z;
        
    }
    
    void BBox::extend(const BBox& bbox)
    {
        if (bbox == empty())
            return;

        min.x = bbox.min.x < min.x ? bbox.min.x : min.x;
        min.y = bbox.min.y < min.y ? bbox.min.y : min.y;
        min.z = bbox.min.z < min.z ? bbox.min.z : min.z;

        max.x = bbox.max.x > max.x ? bbox.max.x : max.x;
        max.y = bbox.max.y > max.y ? bbox.max.y : max.y;
        max.z = bbox.max.z > max.z ? bbox.max.z : max.z;
    }

    float BBox::SurfaceArea() const
    {
        return (max.x - min.x) * (max.y - min.y) * 2 + (max.x - min.x) * (max.z - min.z) * 2 + (max.z - min.z) * (max.y - min.y) * 2;
    }

    uint32_t BBox::MaximumExtent() const
    {
        float dx = max.x - min.x;
        float dy = max.y - min.y;
        float dz = max.z - min.z;

        if (dx > dy && dx > dz)
            return 0;
        else if (dy > dx && dy > dz)
            return 1;
        else
            return 2;
    }

    bool operator==(const BBox& b1, const BBox& b2)
    {
        return b1.min == b2.min && b1.max == b2.max;
    }

    BBox operator+(const BBox& b1, const BBox& b2)
    {
        BBox box = b1;
        box.extend(b2);
        return box;
    }


    void BBox::SetMin(int axis, float val)
    {
        switch (axis)
        {
        case 0:
            min.x = val;
            break;
        case 1:
            min.y = val;
            break;
        case 2:
            min.z = val;
            break;
        }
    }

    void BBox::SetMax(int axis, float val)
    {
        switch (axis)
        {
        case 0:
            max.x = val;
            break;
        case 1:
            max.y = val;
            break;
        case 2:
            max.z = val;
            break;
        }
    }
}

