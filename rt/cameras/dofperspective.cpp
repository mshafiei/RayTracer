#include <rt/cameras/dofperspective.h>
#include <rt/ray.h>
#include <core/scalar.h>
namespace rt
{
    DOFPerspectiveCamera::DOFPerspectiveCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up,
        float verticalOpeningAngle,
        float horizonalOpeningAngle,
        float focalDistance,
        float apertureRadius
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

        this->focalDistance = focalDistance;
        this->apertureRadius = apertureRadius;
    }

    Ray DOFPerspectiveCamera::getPrimaryRay(float x, float y) const
    {
        Ray ray(Point(0, 0, 0),
                Vector(x*xExtent, y*yExtent, 1).normalize());

        if(apertureRadius > 0.0f)
        {
            // check pbrt page 313
            // choose a random point on the aperture disc
            float r = rt::random() * apertureRadius;
            float theta = rt::random() * 2.0f * rt::pi;
            float x = r * std::cos(theta);
            float y = r * std::sin(theta);

            // solve the focal plane equation to find the
            // corresponding point of focus of the current ray
            float ft = focalDistance;
            Point pFocus = ray(ft);

            // modify the ray origine and direction to start from the
            // sample aperture point in the direction to the focus
            // point
            ray.o = Point(x, y, 0.0f);
            ray.d = (pFocus - ray.o).normalize();
        }
        
        ray = cameraToWorld(ray);
        return ray;
    }
}
