#include <rt/cameras/fisheye.h>

namespace rt
{

    FisheyeCamera::FisheyeCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up, float aperture)
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
        this->aperture = aperture;
    }

    // See: http://paulbourke.net/dome/fisheye/
    Ray FisheyeCamera::getPrimaryRay(float x, float y) const
    {
        float r = sqrt(x*x + y*y);
        if(r > 1)
        {
            return Ray(Point(0, 0, 0), Vector(0, 0, 0));
        }
        float phi = atan2(y, x);
        float theta = r * aperture / 2.f;
        return Ray(Point(0, 0, 0), Vector(sinf(theta)*cosf(phi),
                                          sinf(theta)*sinf(phi),
                                          cosf(theta)));
    }
}
