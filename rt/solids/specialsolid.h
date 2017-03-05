#pragma once
#include <rt/solids/solid.h>
#include <core/point.h>

namespace rt {

    class SpecialSolid : public Solid {
    public:
        SpecialSolid() {}
        SpecialSolid(const Point& center, float radius, CoordMapper* texMapper, Material* material);

        virtual BBox getBounds() const;
        virtual Intersection intersect(const Ray& ray, float previousBestDistance = FLT_MAX) const;
        virtual Point sample() const;
        virtual float getArea() const;

    private:
        Point center;
        float radius;
    };

}