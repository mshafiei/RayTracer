#include "rt\solids\cylinder.h"
#include "core\scalar.h"
#include "core\assert.h"
#include "core\point.h"
#include "rt\bbox.h"
#include "rt\intersection.h"
#include "core\float4.h"
using namespace rt;

Cylinder::Cylinder(float rad, float z0, float z1, float phiMax, CoordMapper* texMapper, Material* material) :
Solid(texMapper, material)
{
    radius = rad;
    zmin = min(z0, z1);
    zmax = max(z0, z1);
    this->phiMax = phiMax;
    Assert(phiMax > 0 && phiMax <= 2*pi) << "Value out of range";
}

BBox Cylinder::getBounds() const
{
    Point p1 = Point(-radius, -radius, zmin);
    Point p2 = Point(radius, radius, zmax);
    return BBox(p1, p2);
}

float Cylinder::getArea() const
{
    return (zmax - zmin) * phiMax * radius;
}

Point Cylinder::sample() const
{
    NOT_IMPLEMENTED;
}

bool Cylinder::Quadratic(float A, float B, float C, float *t0, float *t1) const
{

    // Solve quadratic equation for _t_ values
    float discrim = B * B - 4.f * A * C;
    if (discrim <= 0.) return false;
    float rootDiscrim = sqrtf(discrim);

    float q;
    if (B < 0) q = -.5f * (B - rootDiscrim);
    else q = -.5f * (B + rootDiscrim);
    *t0 = q / A;
    *t1 = C / q;
    if (*t0 > *t1) std::swap(*t0, *t1);
    return true;
}

Intersection Cylinder::intersect(const Ray& tmpRay, float previousBestDistance) const
{
    float phi;
    Point phit;
    Ray ray = tmpRay;
    //ray.o = Point(ray.o.x - center.x, ray.o.y - center.y, ray.o.z - center.z);

    // Compute quadratic cylinder coefficients
    bool quadratic = false;
    float A = ray.d.x*ray.d.x + ray.d.y*ray.d.y;
    float B = 2 * (ray.d.x*ray.o.x + ray.d.y*ray.o.y);
    float C = ray.o.x*ray.o.x + ray.o.y*ray.o.y - radius*radius;

    float t0, t1;
    if (!Quadratic(A, B, C, &t0, &t1))
        return Intersection::failure();

    // Compute intersection distance along ray
    float thit = t0;
    if (t0 < 0) 
    {
        thit = t1;
        if (thit < 0) return Intersection::failure();
    }

    // Compute cylinder hit point and $\phi$
    phit = ray(thit);
    phi = atan2f(phit.y, phit.x);
    if (phi < 0.) phi += 2.f*pi;

    // Test cylinder intersection against clipping parameters
    if (phit.z < zmin || phit.z > zmax || phi > phiMax) {
        if (thit == t1) return Intersection::failure();
        thit = t1;
        // Compute cylinder hit point and $\phi$
        phit = ray(thit);
        phi = atan2f(phit.y, phit.x);
        if (phi < 0.) phi += 2.f*pi;
        if (phit.z < zmin || phit.z > zmax || phi > phiMax)
            return Intersection::failure();
    }
    if (thit > previousBestDistance)
        return Intersection::failure();

    // Find parametric representation of cylinder hit
    float u = phi / phiMax;
    float v = (phit.z - zmin) / (zmax - zmin);
    Point hitP = ray.getPoint(thit);
    Intersection ints(thit, ray, this, (Point(0, 0 ,hitP.z) - hitP).normalize(), Point(u, v, 0));
    ints.outDistance = t1;

    return ints;
}