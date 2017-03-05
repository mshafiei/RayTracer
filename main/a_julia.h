#if !defined(A_JULIA_H)

#include <core/point.h>
#include <core/vector.h>
#include <core/color.h>
#include <core/scalar.h>
#include <core/julia.h>
#include <core/image.h>
#include <rt/renderer.h>

using namespace rt;

float a1computeWeight(float fx, float fy, const Point& c, float div);
RGBColor a1computeColor(uint x, uint y, uint width, uint height);
void a_julia();

#define A_JULIA_H
#endif
