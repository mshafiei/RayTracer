#define _USE_MATH_DEFINES
#include <rt/solids/solid.h>
#include <rt/solids/aabox.h>
#include <rt/solids/infiniteplane.h>
#include <rt/solids/quad.h>
#include <rt/solids/sphere.h>
#include <rt/solids/triangle.h>
#include <rt/solids/disc.h>
#include <rt/solids/striangle.h>
#include <rt/bbox.h>
#include <core/assert.h>
#include <rt/intersection.h>
#include <cmath>
#include <core/vector.h>
#include <algorithm>
#include <rt/coordmappers/world.h>
#include <core/random.h>

namespace rt
{
#define getBoundsNotImpelemented(type)              \
    BBox type::getBounds() const {NOT_IMPLEMENTED;}
#define sampleNotImpelemented(type)                 \
    Point type::sample() const {NOT_IMPLEMENTED;}

    Intersection triangleIntersect(const Ray& ray, const Point& p1,
                                   const Vector& e1, const Vector& e2,
                                   const Solid* solid,
                                   float previousBestDistance);

    Intersection planeIntersect(const Ray& ray, const Point& origin,
                                const Vector& normal,
                                const Solid* solid,
                                float previousBestDistance);
    
    Solid::Solid(CoordMapper* texMapper, Material* material)
    {
        this->texMapper = texMapper;
        if (texMapper == nullptr)
        {
            this->texMapper = new WorldMapper(Float4(1, 1, 1, 1));
        }
        this->material = material;
    }

    Vector AABox::normals[3] =
    {
        Vector(-1, 0, 0),
        Vector(0, -1, 0),
        Vector(0, 0, -1)
    };
    
    AABox::AABox(const Point& corner1, const Point& corner2,
                 CoordMapper* texMapper, Material* material)
            : Solid(texMapper, material)
    {
        pMin = min(corner1, corner2);
        pMax = max(corner1, corner2);

        center = Point(pMin.x + pMax.x, pMin.y + pMax.y,
                       pMin.z + pMax.z) * 0.5f;
    }

    BBox AABox::getBounds() const
    {
        return BBox(pMin, pMax);
    }

    // See pbrt book, page 194
    Intersection AABox::intersect(const Ray& ray,
                                  float previousBestDistance) const
    {
        float t0 = 0;
        float t1 = previousBestDistance;
        Vector intersectionNormal;
        
        for(int i=0 ; i<3 ; i++)
        {
            int swapped = 1;
            float invRayDir = 1.f / ray.d[i];
            float tNear = (pMin[i] - ray.o[i]) * invRayDir;
            float tFar = (pMax[i] - ray.o[i]) * invRayDir;

            if(tNear > tFar)
            {
                std::swap(tNear, tFar);
                swapped = -1;
            }

            if(tNear > t0)
            {
                t0 = tNear;
                intersectionNormal = swapped * normals[i];
            }
            
            t1 = tFar < t1 ? tFar : t1;

            if(t0 > t1)
            {
                return Intersection::failure();
            }
        }

        if(previousBestDistance > t0)
        {
            Point p = (ray.o + (ray.d * t0));
            Intersection inter = Intersection(t0, ray, this, intersectionNormal,
                Point(p.x - center.x, p.y - center.y,
                p.z - center.z));
            inter.outDistance = t1;
            return inter;
        }

        return Intersection::failure();
    }

    float AABox::getArea() const
    {
        float xExtent = pMax.x - pMin.x;
        float yExtent = pMax.y - pMin.y;
        float zExtent = pMin.z - pMin.z;
        return ((xExtent*yExtent*2) + (xExtent*zExtent*2) +
                (yExtent*zExtent*2));
    }

    sampleNotImpelemented(AABox)

    InfinitePlane::InfinitePlane(const Point& origin,
                                 const Vector& normal,
                                 CoordMapper* texMapper,
                                 Material* material)
    : origin(origin),
    Solid(texMapper, material)
    {
        this->normal = normal.normalize();
    }

    BBox InfinitePlane::getBounds() const
    {
        return BBox::full();
    }


    Intersection InfinitePlane::intersect(const Ray& ray,
                                          float previousBestDistance) const
    {
        return planeIntersect(ray, origin, normal, this,
                              previousBestDistance);
    }

    float InfinitePlane::getArea() const
    {
        // TODO: should we leave it as NOT_IMPLEMENTED or should we
        // return FLT_MAX?
        return FLT_MAX;
        //NOT_IMPLEMENTED;
    }
    
    sampleNotImpelemented(InfinitePlane)

    Quad::Quad(const Point& v1, const Vector& span1,
               const Vector& span2,
               CoordMapper* texMapper, Material* material)
    : v1(v1), span1(span1), span2(span2),
    Solid(texMapper, material)
    {
        normal = rt::cross(span1, span2).normalize();
    }

    Vector Quad::getNormal() const
    {
        return normal;
    }
        
    
    BBox Quad::getBounds() const
    {
        BBox box = BBox::empty();
        box.extend(v1 + span1);
        box.extend(v1 + span2);
        box.extend(span1 + span2 + v1);
        
        return box;
    }

    Intersection Quad::intersect(const Ray& ray,
                                 float previousBestDistance) const
    {
        Intersection inter =
            triangleIntersect(ray, v1, span1, span2,
                              this, previousBestDistance);
        if(inter)
        {
            inter.solid = this;
            inter.uv = getLocalPoint(inter.uv, v1);
            return inter;
        }
        else
        {
            Point pivot = v1+span1+span2;
            inter =
            triangleIntersect(ray, pivot, -span1, -span2,
                              this, previousBestDistance);
            if(inter)
            {
                inter.solid = this;
                inter.uv = getLocalPoint(inter.uv, pivot);
                return inter;
            }
        }

        return Intersection::failure();
    }

    float Quad::getArea() const
    {
        Vector proj = span1 * dot(span2, span1.normalize());
        Vector heightVec;

        // if span1 and span2 perpendicular to each other heightVec is
        // simply span2
        if(proj.lensqr() <= EPSILON)
        {
            heightVec = span2;
        }
        else
        {
            heightVec = span2 - proj;
        }

        return (span1.length() * heightVec.length());
    }

    Point Quad::sample() const
    {
        // check http://mathworld.wolfram.com/TrianglePointPicking.html
        float a1 = rt::random();
        float a2 = rt::random();
        return (v1 + a1 * span1 + a2 * span2);
    }
    
    Point Quad::getLocalPoint(const Point& uv, const Point& pivot) const
    {
        Vector temp = (pivot + uv.y*span1 + uv.z*span2) - v1;
        return Point(temp.x, temp.y, temp.z);
    }
    
    Sphere::Sphere(const Point& center, const float radius,
        CoordMapper* texMapper, Material* material)
    : center(center), radius(radius),
    Solid(texMapper, material), center2(center)
    {
        
    }

    BBox Sphere::getBounds() const
    {
        BBox bb1 = BBox(center - radius, center + radius);
        bb1.extend(BBox(center2 - radius, center2 + radius));
        return bb1;

    }
    Intersection Sphere::intersect(const Ray& ray,
                                   float previousBestDistance) const
    {
        auto tmp = ray.t * center + (1 - ray.t) * center2;
        Point centerT(tmp[0], tmp[1], tmp[2]);
        Vector rayV = ray.o - centerT;
        float rayL = rayV.length();
        float vDotD = dot(rayV, ray.d);
        float discriminant = vDotD*vDotD - dot(rayV, rayV)
            + (radius*radius);

        if(discriminant < 0)
        {
            return Intersection::failure();
        }

        discriminant = sqrtf(discriminant);
        float t0 = -vDotD - discriminant;

        if(t0 > previousBestDistance)
        {
            return Intersection::failure();
        }

        float t1 = -vDotD + discriminant;
        
        if(t0 < 0)
        {
            t0 = -vDotD + discriminant;

            if(t0 < 0)
            {
                return Intersection::failure();
            }
        }

        Point hitPoint = ray.o + (t0 * ray.d);
        Vector vHit = (hitPoint - centerT);
        Vector normal = vHit.normalize();
        // project vHit on the xz plane (here we use the convention
        // that the equator of the sphere is in the xz plane since y
        // is the up vector)
        static Vector up(0, 1, 0);
        static Vector right(0, 0, 1);
        Vector xzProj = (vHit - dot(vHit, up) * up).normalize();
        float theta = dot(xzProj, right);
        float phi = dot(normal, up);
        float u = phi / (2 * M_PI);
        float v = theta / (M_PI);
        //Intersection inter = Intersection(t0, ray, this, normal, Point(u, v, 0));
        Intersection inter = Intersection(t0, ray, this, normal, Point(vHit[0], vHit[1], vHit[2]));
        inter.outDistance = t1;
        return inter;
    }

    float Sphere::getArea() const
    {
        return (4 * M_PI * radius * radius);
    }

    sampleNotImpelemented(Sphere)

    Triangle::Triangle(Point vertices[3],
                       CoordMapper* texMapper, Material* material)
    : Solid(texMapper, material)
    {
        this->vertices[0] = vertices[0];
        this->vertices[1] = vertices[1];
        this->vertices[2] = vertices[2];
    }

    Triangle::Triangle(const Point& v1, const Point& v2,
                       const Point& v3,
                       CoordMapper* texMapper, Material* material)
            : Solid(texMapper, material)
    {
        this->vertices[0] = v1;
        this->vertices[1] = v2;
        this->vertices[2] = v3;
    }
    
    BBox Triangle::getBounds() const
    {
        BBox box = BBox::empty();
        box.extend(vertices[0]);
        box.extend(vertices[1]);
        box.extend(vertices[2]);
        return box;

    }

    Intersection Triangle::intersect(const Ray& ray,
                                     float previousBestDistance) const
    {
        Vector e1 = vertices[1] - vertices[0];
        Vector e2 = vertices[2] - vertices[0];
        return triangleIntersect(ray, vertices[0], e1, e2,
                                 this, previousBestDistance);
    }

    float Triangle::getArea() const
    {
        Vector span1 = vertices[1] - vertices[0];
        Vector span2 = vertices[2] - vertices[0];

        Vector proj = span1 * dot(span2, span1.normalize());
        Vector heightVec;

        // if span1 and span2 perpendicular to each other heightVec is
        // simply span2
        if(proj.lensqr() <= EPSILON)
        {
            heightVec = span2;
        }
        else
        {
            heightVec = span2 - proj;
        }

        return (span1.length() * heightVec.length() * 0.5f);
    }

    Point Triangle::sample() const
    {
        // check
        // http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-2d-triangle
        // second answer
        Point p0, p1, p2;
        p0 = vertices[0];
        p1 = vertices[1];
        p2 = vertices[2];

        // sample a point on the quad containing the triangle
        float a1 = random();
        float a2 = random();
        Point p = (p0 + a1 * (p1 - p0) + a2 * (p2 - p0));

        // find whether the sampled point is inside the triangle ...
        // calculate the triangle area
        float A = 0.5f * (-p1.y * p2.x
                        + p0.y * (-p1.x + p2.x)
                        + p0.x * (p1.y - p2.y)
                        + p1.x * p2.y);
        float sign = A < 0 ? -1 : 1;
        // calculate bary-centric coordinates
        float s = (p0.y * p2.x
                   - p0.x * p2.y
                   + (p2.y - p0.y) * p.x
                   + (p0.x - p2.x) * p.y) * sign;

        float t = (p0.x * p1.y
                 - p0.y * p1.x
                 + (p0.y - p1.y) * p.x
                 + (p1.x - p0.x) * p.y) * sign;

        // if inside the triangles (bary-centric coords are positive
        // and add up to 1)
        if(s > 0 && t > 0 && (s + t) < 2 * A * sign)
        {
            return p;
        }
        // else search for another point
        else
        {
            // TODO try to make that more efficient if the performance
            // is bad!
            return sample();
        }
    }

    Disc::Disc(const Point& center, const Vector& normal,
               float radius,
               CoordMapper* texMapper, Material* material)
    : center(center), radius(radius),
    Solid(texMapper, material)
    {
        this->normal = normal;
    }
    
    BBox Disc::getBounds() const
    {
        return BBox(Point(center.x - radius, center.y - radius, center.z - radius),
            Point(center.x + radius, center.y + radius, center.z + radius));
    }

    Intersection Disc::intersect(const Ray& ray,
                                 float previousBestDistance) const
    {
        Intersection planeInter = planeIntersect(ray, center, normal,
                                                 this,
                                                 previousBestDistance);
        Point pInter = ray(planeInter.distance);
        Vector diff = pInter - center;
        float distSqrd = diff.lensqr();

        if(distSqrd > (radius*radius))
        {
            return Intersection::failure();
        }

        // this is really expensive now, try to find a better way

        // a cross product will produce a vector perpendicular to normal
        // and hence in the disc plane
        Vector v1 = cross(normal, Vector(1, 0, 0)).normalize();
        // now v1 can be used as a refernce vector against which we
        // can estimate phi
        float v = dot(v1, diff) / (2.f * M_PI);
        float u = sqrtf(distSqrd) / radius;
        planeInter.uv = Point(u, v, 0.f);

        return planeInter;
    }

    float Disc::getArea() const
    {
        return (M_PI * radius * radius);
    }

    sampleNotImpelemented(Disc)

    SmoothTriangle::SmoothTriangle(Point vertices[3],
                                   Vector normals[3],
                                   CoordMapper* texMapper,
                                   Material* material)
    : Triangle(vertices, texMapper, material)
    {
        this->normals[0] = normals[0].normalize();
        this->normals[1] = normals[1].normalize();
        this->normals[2] = normals[2].normalize();
    }

    SmoothTriangle::SmoothTriangle(const Point& v1, const Point& v2,
                                   const Point& v3, const Vector& n1,
                                   const Vector& n2, const Vector& n3, 
        CoordMapper* texMapper, Material* material)
            : Triangle(v1, v2, v3, texMapper, material)
    {
        normals[0] = n1.normalize();
        normals[1] = n2.normalize();
        normals[2] = n3.normalize();
    }

    Intersection SmoothTriangle::intersect(const Ray& ray,
                                           float previousBestDistance) const
    {
        Vector e1 = vertices[1] - vertices[0];
        Vector e2 = vertices[2] - vertices[0];
        Intersection inter = triangleIntersect(ray, vertices[0],
                                               e1, e2, this,
                                               previousBestDistance);
        if(inter)
        {
            // interpolate the normals according to the barycentric
            // coordinates
            inter.norm = ((inter.uv[0] * normals[0]) +
                          (inter.uv[1] * normals[1]) +
                          (inter.uv[2] * normals[2]));
        }

        return inter;
    }

    // See pbrt book, page 140
    // A helper function used both in Quad::intersect and
    // Triangle::intersect functions
    Intersection triangleIntersect(const Ray& ray, const Point& p1,
                                   const Vector& e1, const Vector& e2,
                                   const Solid* solid,
                                   float previousBestDistance)
    {
        // t1 is the same t0
        Vector s1 = cross(ray.d, e2);
        float divisor = dot(s1, e1);

        if(divisor == 0.f)
        {
            return Intersection::failure();
        }

        float invDivisor = 1.f / divisor;
        Vector d = ray.o - p1;
        float b1 = dot(d, s1) * invDivisor;

        if(b1 < 0.f || b1 > 1.f)
        {
            return Intersection::failure();
        }

        Vector s2 = cross(d, e1);
        float b2 = dot(ray.d, s2) * invDivisor;

        if(b2 < 0.f || (b1 + b2 > 1.f))
        {
            return Intersection::failure();
        }

        float t = dot(e2, s2) * invDivisor;

        if(t < 0 || t > previousBestDistance)
        {
            return Intersection::failure();
        }

        Intersection inter = Intersection(t, ray, solid, cross(e2, e1).normalize(),
                                          Point(1 - b1 - b2, b1, b2));
        inter.outDistance = t;
        return inter;
        
    }

    // t1 will be the same as t0
    Intersection planeIntersect(const Ray& ray, const Point& origin,
                                const Vector& normal,
                                const Solid* solid,
                                float previousBestDistance)
    {
        float deno = dot(ray.d, normal);

        if(deno != 0)
        {
            Vector rayPlaneVec = ray.o - origin;
            float t = -dot(rayPlaneVec, normal);
            t = t / deno;

            if(t >= 0 && t < previousBestDistance)
            {
                Point uv = ray.o + (ray.d * t);
                uv = Point(uv.x-origin.x, uv.y-origin.y,
                           uv.z-origin.z);
                Intersection inter = Intersection(t, ray, solid, normal, uv);
                inter.outDistance = t;
                return inter;
            }
        }

        return Intersection::failure();
    }
}
