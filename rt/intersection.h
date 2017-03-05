#ifndef CG1RAYTRACER_INTERSECTION_HEADER
#define CG1RAYTRACER_INTERSECTION_HEADER

#include <core/scalar.h>
#include <core/vector.h>
#include <rt/ray.h>
#include <vector>

namespace rt {

class Solid;

class Intersection {
public:
    Ray ray;
    const Solid* solid;
    float distance;
    float outDistance;
    Point uv;
    Vector norm;
    
    Intersection() { }
    static Intersection failure();
    Intersection(float distance, const Ray& ray, const Solid* solid, const Vector& normal, const Point& uv);
    //Intersection(const Intersection& i);

    Point hitPoint() const;
    Vector normal() const;
    Point local() const;

    operator bool(); //this allows intersection object to be put directly in conditional statements. Becomes true iff there is an intersection
    
};

Intersection operator&(const Intersection& i1, const Intersection& i2);
std::vector<Intersection> operator|(const Intersection& i1, const Intersection& i2);
std::vector<Intersection> operator-(const Intersection& i1, const Intersection& i2);
bool operator==(const Intersection& i1, bool flag); //this allows intersection object to be put directly in conditional statements. Becomes true iff there is an intersection
}

#endif
