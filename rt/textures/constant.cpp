#include <rt/textures/constant.h>
using namespace rt;
ConstantTexture::ConstantTexture(){}
ConstantTexture::ConstantTexture(const RGBColor& color):color(color){}

RGBColor ConstantTexture::getColor(const Point& coord)
{ 
    return color;
}

RGBColor ConstantTexture::getColorDX(const Point& coord)
{
    return RGBColor::rep(0);
}

RGBColor ConstantTexture::getColorDY(const Point& coord)
{
    return RGBColor::rep(0);
}
