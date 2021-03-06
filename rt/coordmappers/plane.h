#ifndef CG1RAYTRACER_COORDMAPPERS_PLANE_HEADER
#define CG1RAYTRACER_COORDMAPPERS_PLANE_HEADER

#include <core/vector.h>
#include <rt/coordmappers/coordmapper.h>
#include <core/point.h>
namespace rt {

class Intersection;

class PlaneCoordMapper : public CoordMapper{
public:
    PlaneCoordMapper(const Point& origin, const Vector& e1, const Vector& e2);
    virtual Point getCoords(const Intersection& hit) const;
    Vector e1;
    Vector e2;
    Point origin;
};

}

#endif
