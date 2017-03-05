#if !defined(FISHEYE_H)

#include <rt/cameras/camera.h>
#include <core/vector.h>
#include <core/point.h>
#include <core/transform.h>
#include <rt/cameras/perspective.h>
#include <core/image.h>
#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#else
#include <cmath>
#endif

namespace rt
{
    class Ray;
    
    class FisheyeCamera : public Camera
    {
    public:
        FisheyeCamera(const Point& center,
                      const Vector& forward,
                      const Vector& up, float aperture=2*M_PI);

        virtual Ray getPrimaryRay(float x, float y) const;
        
    private:
        Transform worldToCamera;
        Transform cameraToWorld;
        Transform cameraToScreen;
        float aperture;
    };
}

#define FISHEYE_H
#endif
