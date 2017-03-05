#pragma once
#include "rt\solids\solid.h"
#include "core/point.h"
using namespace rt;

class Cylinder : public Solid {

public:
    Cylinder(float rad, float z0, float z1, float pm, CoordMapper* texMapper, Material* material);

    virtual BBox getBounds() const;
    virtual Intersection intersect(const Ray& ray, float previousBestDistance = FLT_MAX) const;
    virtual Point sample() const;
    virtual float getArea() const;
    bool Quadratic(float A, float B, float C, float *t0, float *t1) const;
    Intersection Block(int n, int m);

private:
    float radius;
    float zmin;
    float zmax;
    float phiMax;

};