#include "interpolate.h"
#include "assert.h"

namespace rt {

    template <typename T>
    T lerp(const T& px0, const T& px1, float xPoint) {
        Assert(xPoint <= 1 && xPoint >= 0) << "xPoint violates the conditions";
        return (1 - xPoint) * px0 + xPoint * px1;
    }

    template <typename T>
    T lerpbar(const T& a, const T& b, const T& c, float aWeight, float bWeight) {
        return aWeight * a + bWeight * b + (1 - aWeight - bWeight) * c;
    }

    template <typename T>
    T lerp2d(const T& px0y0, const T& px1y0, const T& px0y1, const T& px1y1, float xWeight, float yWeight) {
        T fy1 = (1 - xWeight) * px0y0 + xWeight * px1y0;
        T fy2 = (1 - xWeight) * px0y1 + xWeight * px1y1;
        return (1 - yWeight) * fy1 + yWeight * fy2;
    }

    template <typename T>
    T lerp3d(const T& px0y0z0, const T& px1y0z0, const T& px0y1z0, const T& px1y1z0,
        const T& px0y0z1, const T& px1y0z1, const T& px0y1z1, const T& px1y1z1,
        float xPoint, float yPoint, float zPoint) {
        T x1 = (1 - xPoint) * px0y0z0 + xPoint * px1y0z0;
        T x2 = (1 - xPoint) * px0y1z0 + xPoint * px1y1z0;
        T x3 = (1 - xPoint) * px0y0z1 + xPoint * px1y0z1;
        T x4 = (1 - xPoint) * px0y1z1 + xPoint * px1y1z1;
        T y1 = (1 - yPoint) * x1 + yPoint * x2;
        T y2 = (1 - yPoint) * x3 + yPoint * x4;
        return (1 - zPoint) * y1 + zPoint * y2;
    }

}

