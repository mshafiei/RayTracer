#ifndef CG1RAYTRACER_CAMERAS_PERSPECTIVE_HEADER
#define CG1RAYTRACER_CAMERAS_PERSPECTIVE_HEADER

#include <rt/cameras/camera.h>
#include <core/vector.h>
#include <core/point.h>
#include <core/transform.h>

namespace rt {

    class Ray;
    
class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up,
        float verticalOpeningAngle,
        float horizonalOpeningAngle
        );

    virtual Ray getPrimaryRay(float x, float y) const;

protected:
    Transform worldToCamera;
    Transform cameraToWorld;
    Transform cameraToScreen;
    float verticalOpeningAngle;
    float horizonalOpeningAngle;
    float xExtent, yExtent;
};

}


#endif
