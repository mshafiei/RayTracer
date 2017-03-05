#include <rt/cameras/orthographic.h>
#include <rt/ray.h>
#include <core/transform.h>

namespace rt
{
    OrthographicCamera::OrthographicCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up,
        float scaleX,
        float scaleY
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
        cameraToScreen = Orthographic(0.f, 1.f);
        this->scaleX = scaleX/2;
        this->scaleY = scaleY/2;
    }

    Ray OrthographicCamera::getPrimaryRay(float x, float y) const
    {
        Ray ray(Point(x*scaleX, y*scaleY, 0), Vector(0, 0, 1));
        ray = cameraToWorld(ray);
        return ray;
    }
}
