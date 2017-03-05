#if !defined(A_CAMERAS_H)

#include <core/image.h>
#include <core/point.h>
#include <core/vector.h>
#include <core/scalar.h>
#include <core/julia.h>
#include <rt/ray.h>
#include <rt/cameras/perspective.h>
#include <rt/cameras/orthographic.h>
#include <rt/cameras/fisheye.h>
#include <rt/cameras/environment.h>
#include <iostream>
#include <rt/renderer.h>

using namespace rt;

float a2computeWeight(float fx, float fy, const Point& c, float div);
RGBColor a2computeColor(const Ray& r);
void a_cameras();

#define A_CAMERAS_H
#endif
