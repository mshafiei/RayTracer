#include <rt/intersection.h>
#include <core/assert.h>

namespace rt
{
    Intersection Intersection::failure()
    {
        Intersection noInter = {};
        noInter.solid = nullptr;
        noInter.distance = FLT_MAX;
        return noInter;
    }

    Intersection::Intersection(float distance, const Ray& ray,
                               const Solid* solid,
                               const Vector& normal, const Point& uv)
            : distance(distance), ray(ray), solid(solid),
              norm(normal), uv(uv)
    {
    }
    /*
    Intersection::Intersection(const Intersection& i)
    {
        distance = i.distance;
        ray = i.ray;
        solid = i.solid;
        norm = i.norm;
        uv = i.uv;
    }
    */
    Point Intersection::hitPoint() const
    {
        return ray.getPoint(distance);
    }

    Vector Intersection::normal() const
    {
        return norm;
    }

    Point Intersection::local() const
    {
        return uv;
    }
    
    Intersection::operator bool()
    {
        return solid != nullptr;
    }
    bool operator==(const Intersection& i1, bool flag)
    {
        return (i1.distance != FLT_MAX) == flag;
        
    }

    Intersection operator&(const Intersection& i1, const Intersection& i2)
    {
        //assuming that there is always an intersection
        if (i1.distance > i2.outDistance|| i2.distance > i1.outDistance)
            return Intersection::failure();
        
        Intersection i3;

        if (i1.distance > i2.distance)
        {
            i3 = Intersection(i1);
        }
        else
        {
            i3 = Intersection(i2);
        }

        if (i1.outDistance > i2.outDistance)
        {
            i3.outDistance = i2.outDistance;
        }

        return i3;
    }


    std::vector<Intersection> operator|(const Intersection& i1, const Intersection& i2)
    {
        std::vector<Intersection> intersections;
        Intersection i3;
        Intersection i4 = Intersection::failure();
        if (i1 == true && i2 == false)
            i3 = Intersection(i1);
        else if (i1 == false && i2 == true)
            i3 = Intersection(i2);
        else if (i1.distance < i2.distance && i1.outDistance> i2.outDistance)
            i3 = Intersection(i1);
        else if (i2.distance < i1.distance && i2.outDistance> i1.outDistance)
            i3 = Intersection(i2);
        else if (i1.distance < i2.distance)
        {
            i3 = Intersection(i1);
            if (i1.outDistance > i2.distance)
                i3.outDistance = i2.outDistance;
            else
                i4 = Intersection(i2);
        }
        else
        {
            i3 = Intersection(i2);
            if (i2.outDistance > i1.distance)
                i3.outDistance = i1.outDistance;
            else
                i4 = Intersection(i1);
        }
        intersections.push_back(i3);
        if (i4)
            intersections.push_back(i4);

        return intersections;
    }

    std::vector<Intersection> operator-(const Intersection& i1, const Intersection& i2)
    {
        
        Intersection i3 = Intersection::failure();
        std::vector<Intersection> inters;
        
        if (i1.distance < i2.distance && i1.outDistance > i2.outDistance)
        {
            i3 = i1;
            i3.outDistance = i2.distance;
            inters.push_back(i3);
            i3 = i1;
            i3.distance = i2.outDistance;
            i3.outDistance = i1.outDistance;
            inters.push_back(i3);

        }
        else if (i2.distance < i1.distance && i2.outDistance > i1.outDistance)
        {
            inters.push_back(i3);
        }
        else if (i2.distance > i1.outDistance || i2.outDistance < i1.distance)// no intersection
        {
            inters.push_back(i1);
        }
        else if (i2.outDistance > i1.outDistance && i2.distance > i1.distance) // intersection on the edge
        {
            i3 = i1;
            i3.distance = i1.distance;
            i3.outDistance = i2.distance;
            inters.push_back(i3);
        }
        else if (i2.outDistance < i1.outDistance && i2.distance < i1.distance) // intersection on the edge
        {
            i3 = i1;
            i3.distance = i2.outDistance;
            i3.outDistance = i1.outDistance;
            inters.push_back(i3);
        }
        
        return inters;
    }
}