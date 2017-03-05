#ifndef CG1RAYTRACER_BBOX_HEADER
#define CG1RAYTRACER_BBOX_HEADER

#include <utility>
#include <core/point.h>
#include <core/vector.h>
#include <core/assert.h>
#include <core/windowsdefines.h>
namespace rt {

class Ray;

class BBox {
public:
    Point min, max;
    BBox() { }
    BBox(const Point& min, const Point& max);
    static BBox empty();
    static BBox full();

    void extend(const Point& point);
    void extend(const BBox& bbox);

    Vector diagonal() const { return max - min; }

    std::pair<float, float> intersect(const Ray& ray) const;

    bool isUnbound() {NOT_IMPLEMENTED;}

    float SurfaceArea()  const;
    uint32_t MaximumExtent() const;

    void SetMin(int axis, float val);
    void SetMax(int axis, float val);
};

bool operator==(const BBox& b1,const BBox& b2);
BBox operator+(const BBox& b1, const BBox& b2);
}


#endif
