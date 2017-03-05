#include "rt/textures/checkerboard.h"
#include "core/point.h"
#include "core/assert.h"
using namespace rt;

CheckerboardTexture::CheckerboardTexture(const RGBColor& white, const RGBColor& black) :white(white), black(black), edgeLength(0.5f)
{

}

RGBColor CheckerboardTexture::getColor(const Point& coord)
{
    int x = floor(coord.x / edgeLength);
    int y = floor(coord.y / edgeLength);
    int z = floor(coord.z / edgeLength);
    int parity = (x + y + z) % 2;
    if (parity == 0)
        return white;
    else
        return black;
}

RGBColor CheckerboardTexture::getColorDX(const Point& coord)
{
    NOT_IMPLEMENTED;//because we do not know the delta x! coord.x - what??
}

RGBColor CheckerboardTexture::getColorDY(const Point& coord)
{
    NOT_IMPLEMENTED;//because we do not know the delta x! coord.x - what??
}