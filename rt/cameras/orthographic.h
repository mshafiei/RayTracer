#ifndef CG1RAYTRACER_CAMERAS_ORTHOGRAPHIC_HEADER
#define CG1RAYTRACER_CAMERAS_ORTHOGRAPHIC_HEADER

#include <rt/cameras/camera.h>
#include <core/vector.h>
#include <core/point.h>
#include <core/transform.h>

namespace rt {

    class Ray;
    
class OrthographicCamera : public Camera {
public:
    OrthographicCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up,
        float scaleX,
        float scaleY
        );

    virtual Ray getPrimaryRay(float x, float y) const;

private:
    Transform worldToCamera;
    Transform cameraToWorld;
    Transform cameraToScreen;
    float scaleX, scaleY;
};

}


#endif
