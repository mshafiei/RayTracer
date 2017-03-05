#include <rt/cameras/perspective.h>
#include <rt/ray.h>

namespace rt
{
    PerspectiveCamera::PerspectiveCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up,
        float verticalOpeningAngle,
        float horizonalOpeningAngle
        )
    {
        Vector _forward = forward.normalize();
        Vector _right = cross(_forward, up).normalize();
        Vector _up = cross(_right, _forward).normalize();
        Transform trans = Translate(Point(0, 0, 0) - center);
        Matrix4x4 rotMat(_right.x, _right.y, _right.z, 0.f,
                         _up.x, _up.y, _up.z, 0.f,
                         _forward.x, _forward.y, _forward.z, 0.f,
                         0.f, 0.f, 0.f, 1.f);
        Transform rot(rotMat);
        worldToCamera = rot * trans;
        cameraToWorld = Transform(worldToCamera.GetInverseMatrix(),
                                  worldToCamera.GetMatrix());
        this->verticalOpeningAngle = verticalOpeningAngle;
        this->horizonalOpeningAngle = horizonalOpeningAngle;
        yExtent = tan(verticalOpeningAngle/2.f);
        xExtent = tan(horizonalOpeningAngle/2.f);
    }

    Ray PerspectiveCamera::getPrimaryRay(float x, float y) const
    {
        Ray ray(Point(0, 0, 0), Vector(x*xExtent, y*yExtent, 1).normalize());
        ray = cameraToWorld(ray);
        return ray;
    }
}
