#ifndef CG1RAYTRACER_RAY_HEADER
#define CG1RAYTRACER_RAY_HEADER

#include <core/vector.h>
#include <core/point.h>

namespace rt {

class Ray {
public:
    Point o;
    Vector d;
    float t;

    Ray():t(0) {}
    Ray(const Point& o, const Vector& d);
    Point getPoint(float distance) const;
    Point operator()(float distance) const;
    void setT(float t){ this->t = t; }
    
};

}

#endif
