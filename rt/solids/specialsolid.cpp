#include "specialsolid.h"
#include "core/assert.h"
#include "rt/bbox.h"
#include "rt/intersection.h"

using namespace rt;

SpecialSolid::SpecialSolid(const Point& center, const float radius,
    CoordMapper* texMapper, Material* material)
    : center(center), radius(radius),
    Solid(texMapper, material)
{

}

BBox SpecialSolid::getBounds() const
{
    return BBox(center - radius, center + radius);

}

Intersection SpecialSolid::intersect(const Ray& ray,
    float previousBestDistance) const
{
    Vector rayV = ray.o - center;
    float vDotD = dot(rayV, ray.d);
    float discriminant = vDotD*vDotD - dot(rayV, rayV)
        + (radius*radius);

    if (discriminant < 0)
    {
        return Intersection::failure();
    }

    discriminant = sqrtf(discriminant);
    float t0 = -vDotD - discriminant;

    if (t0 > previousBestDistance)
    {
        return Intersection::failure();
    }

    float t1 = -vDotD + discriminant;

    if (t0 < 0)
    {
        t0 = -vDotD + discriminant;

        if (t0 < 0)
        {
            return Intersection::failure();
        }
    }

    Vector vHit = ((ray.o + (t0 * ray.d)) - center);
    Vector normal = vHit.normalize();
    // project vHit on the xz plane (here we use the convention
    // that the equator of the sphere is in the xz plane since y
    // is the up vector)
    static Vector up(0, 1, 0);
    static Vector right(0, 0, 1);
    Vector xzProj = (vHit - dot(vHit, up) * up).normalize();
    float theta = dot(xzProj, right);
    float phi = dot(normal, up);
    float u = phi / (2 * pi);
    float v = theta / (pi);
    //Intersection inter = Intersection(t0, ray, this, normal, Point(u, v, 0));
    Intersection inter = Intersection(t0, ray, this, normal, Point(vHit[0], vHit[1], vHit[2]));
    inter.outDistance = t1;
    return inter;
}

float SpecialSolid::getArea() const
{
    return (4 * pi * radius * radius);
}

Point SpecialSolid::sample() const { NOT_IMPLEMENTED; }
