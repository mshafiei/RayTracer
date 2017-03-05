#ifndef CG1RAYTRACER_CAMERAS_ENVIRONMENT_HEADER
#define CG1RAYTRACER_CAMERAS_ENVIRONMENT_HEADER

#include <rt/cameras/camera.h>
#include <core/vector.h>
#include <core/point.h>
#include <core/transform.h>

namespace rt {

    class Ray;
    
class EnvironmentCamera : public Camera {
public:
    EnvironmentCamera(
        const Point& center,
        const Vector& forward,
        const Vector& up
        );

    // ndcX & ndcY are in normalized device cooridantes NOT in camera
    // space. the range here is from (0, 0) to (1, 1), with (0, 0)
    // being the upper left corner of the image plane
    virtual Ray getPrimaryRay(float ndcX, float ndcY) const;

private:
    Transform worldToCamera;
    Transform cameraToWorld;
    Transform cameraToScreen;
};

}


#endif
