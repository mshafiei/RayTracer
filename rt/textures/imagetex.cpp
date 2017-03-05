#include "rt/textures/imagetex.h"
#include "core/interpolate.h"
#include "core/point.h"
using namespace rt;

ImageTexture::ImageTexture()
{

}

ImageTexture::ImageTexture(const Image& image, BorderHandlingType bh, InterpolationType i) :border(bh), interpol(i)
{
    this->image = image;
}

ImageTexture::ImageTexture(const std::string& filename, BorderHandlingType bh, InterpolationType i ) : border(bh), interpol(i)
{
    image.readPNG(filename);
}

RGBColor ImageTexture::getColor(const Point& coord)
{
    float u = coord.x;// / image.width();
    float v = coord.y;// / image.height();
    int intu = floor(u);
    int intv = floor(v);
    float tu = u - intu;
    float tv = v - intv;
    if (border == CLAMP)
    {
        if (u < 0)
            tu = 0;
        if (u > 1)
            tu = 1;

        if (v < 0)
            tv = 0;
        if (v > 1)
            tv = 1;
    }
    else if (border == MIRROR)
    {
        if (intu % 2 != 0)
            tu = 1 - tu;

        if (intv % 2 != 0)
            tv = 1 - tv;

    }
    else //REPEAT
    {
        /*coord.x = fmodf(coord.x, image.width()) * (coord.x < 0 ? -1.0f : 1.0f);
        coord.y = fmodf(coord.y, image.height()) * (coord.y < 0 ? -1.0f : 1.0f);
*/
    }

    if (interpol == BILINEAR)
    {
        int bx = 1;
        int by = 1;

        int intx = floor(tu * image.width());
        int inty = floor(tv * image.height());

        if (intx >= image.width() - 1)
        {
            bx = 0;
            intx = image.width() - 1;
        }
        if (inty >= image.height() - 1)
        {
            by = 0;
            inty = image.height() - 1;
        }
        
        float fltx = tu * image.width() - intx;
        float flty = tv * image.height() - inty;
        RGBColor cx0y0 = image(intx,inty);
        RGBColor cx1y0 = image(intx + bx, inty);
        RGBColor cx0y1 = image(intx, inty + by);
        RGBColor cx1y1 = image(intx + bx, inty + by);
        RGBColor p1 = (1 - fltx) * cx0y0 + fltx * cx1y0;
        RGBColor p2 = (1 - fltx) * cx0y1 + fltx * cx1y1;
        return (1 - flty) * p1 + flty * p2;
        return cx0y0;
    }
    else //Nearest
    {
        int intx = floor(tu * image.width());
        int inty = floor(tv * image.height());
        int lu = std::min(intx, (int)image.width() - 1);
        int lv = std::min(inty, (int)image.height() - 1);
        return image(lu, lv);
    }
}

float scale = 0.002f;

// this approach this verified by PBRT's suggestion at page 494: "...
// For example, for image map textures mapped to the surface directly
// using its (u, v) parameterization, these partial derivatives can be
// computed by subtracting adjacent texels in the u and v
// directions. ..."
RGBColor ImageTexture::getColorDX(const Point& coord)
{
    // calculate forward differences
    Point coord2 = coord;
    coord2.x = coord2.x - scale;
    RGBColor c1 = getColor(coord);
    RGBColor c2 = getColor(coord2);
    return (c2 - c1) / scale;
}

RGBColor ImageTexture::getColorDY(const Point& coord)
{
    Point coord2 = coord;
    coord2.y = coord2.y + scale;
    RGBColor c1 = getColor(coord);
    RGBColor c2 = getColor(coord2);
    return (c2 - c1) / scale;
}
