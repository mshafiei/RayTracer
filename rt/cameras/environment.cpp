#define _USE_MATH_DEFINES
#include <rt/cameras/environment.h>
#include <rt/ray.h>
#include <core/transform.h>

namespace rt
{
    EnvironmentCamera::EnvironmentCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up)

    {
        Vector _forward = forward.normalize();
        Vector _right = cross(_forward, up).normalize();
        Vector _up = cross(_forward, _right).normalize();
        Transform trans = Translate(Point(0, 0, 0) - center);
        Matrix4x4 rotMat(_right.x, _right.y, _right.z, 0.f,
                         _up.x, _up.y, _up.z, 0.f,
                         _forward.x, _forward.y, _forward.z, 0.f,
                         0.f, 0.f, 0.f, 1.f);
        Transform rot(rotMat);
        worldToCamera = rot * trans;
        cameraToWorld = Transform(worldToCamera.GetInverseMatrix(),
                                  worldToCamera.GetMatrix());
    }

    // See PBRT book page 319
    Ray EnvironmentCamera::getPrimaryRay(float ndcX, float ndcY) const
    {
        float theta = M_PI * (ndcY+1)/2;
        float phi = 2 * M_PI * (ndcX+1)/2;
        Vector dir(sinf(theta)*cosf(phi), cosf(theta),
                   sinf(theta)*sinf(phi));
        Ray ray(Point(0,0,0), dir);
        ray = cameraToWorld(ray);
        return ray;
    }
}
