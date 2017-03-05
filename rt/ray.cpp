#include <rt/ray.h>
#include <math.h>
namespace rt
{
    Ray::Ray(const Point& o, const Vector& d)
    {
        this->o = o;
        this->d = d.normalize();
        this->t = 0;
    }

    Point Ray::getPoint(float distance) const
    {
        return o + (d * distance);
    }

    Point Ray::operator()(float distance) const
    {
        return getPoint(distance);
    }

}
