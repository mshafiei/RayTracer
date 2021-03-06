#ifndef CG1RAYTRACER_CAMERAS_DOFPERSPECTIVE_HEADER
#define CG1RAYTRACER_CAMERAS_DOFPERSPECTIVE_HEADER

#include <rt/cameras/camera.h>
#include <core/vector.h>
#include <core/point.h>
#include <core/random.h>
#include <math.h>
#include <core/vector.h>
#include <core/transform.h>

namespace rt {

class DOFPerspectiveCamera : public Camera {
public:
    DOFPerspectiveCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up,
        float verticalOpeningAngle,
        float horizonalOpeningAngle,
        float focalDistance,
        float apertureRadius
        );

    virtual Ray getPrimaryRay(float x, float y) const;

private:
    Transform worldToCamera;
    Transform cameraToWorld;
    Transform cameraToScreen;
    float verticalOpeningAngle;
    float horizonalOpeningAngle;
    float xExtent, yExtent;
    float focalDistance;
    float apertureRadius;
};

}


#endif
