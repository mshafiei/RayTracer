#include "rt/coordmappers/tmapper.h"
#include "rt/intersection.h"
using namespace rt;

TriangleMapper::TriangleMapper(const Point& tv0, const Point& tv1, const Point& tv2) :tv0(tv0), tv1(tv1), tv2(tv2)
{

}

TriangleMapper::TriangleMapper(Point ntv[3]) : tv0(ntv[0]), tv1(ntv[1]), tv2(ntv[2])
{ 

}

Point TriangleMapper::getCoords(const Intersection& hit) const
{ 
    Point bary = hit.local();
    //Point res = bary.z * tv0 + bary.x * tv1 + bary.y * tv2;
    Point res = bary.x * tv0 + bary.y * tv1 + bary.z * tv2;
    return res;
}