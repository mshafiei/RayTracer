#include "rt/coordmappers/plane.h"
#include "rt/intersection.h"
#include "core/matrix.h"
using namespace rt;
PlaneCoordMapper::PlaneCoordMapper(const Point& origin,const Vector& e1, const Vector& e2) :origin(origin),e1(e1), e2(e2)
{

}

Point PlaneCoordMapper::getCoords(const Intersection& hit) const
{
    Vector norm = cross(e1, e2).normalize();
    Vector e1n = e1.normalize();
    Vector e2n = e2.normalize();
    Point p = hit.hitPoint() - dot(hit.hitPoint() - origin, norm) * norm;
    Matrix mat;
        mat[0][0] = e1n.x; mat[0][1] = e2n.x; mat[0][2] = norm.x; mat[0][3] = 0;
        mat[1][0] = e1n.y; mat[1][1] = e2n.y; mat[1][2] = norm.y; mat[1][3] = 0;
        mat[2][0] = e1n.z; mat[2][1] = e2n.z; mat[2][2] = norm.z; mat[2][3] = 0;
        mat[3][0] = 0;     mat[3][1] = 0;     mat[3][2] = 0;      mat[3][3] = 1;
    if (dot(e1, e2) == 0.f)
    {
        mat = mat.transpose();
    }
    else
    {
        mat = mat.invert();
    }
    //return p;
    Point res = mat * p;
    float s = 1;// (p.x * mat[2][0] + p.y *  mat[2][1] + p.z * mat[2][2]);
    return Point(res.x / (s*e1.length()), res.y / (s*e2.length()), 0);
}