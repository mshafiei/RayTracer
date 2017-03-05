#include <core/color.h>
#include <algorithm>

namespace rt
{
    // Try using a Macro here
    RGBColor RGBColor::operator+(const RGBColor& c) const
    {
        return RGBColor(r+c.r, g+c.g, b+c.b);
    }
    
    RGBColor RGBColor::operator-(const RGBColor& c) const
    {
        return RGBColor(r-c.r, g-c.g, b-c.b);
    }

    RGBColor RGBColor::operator*(const RGBColor& c) const
    {
        return RGBColor(r*c.r, g*c.g, b*c.b);
    }
    
    bool RGBColor::operator==(const RGBColor& b) const
    {
        return ((this->r == b.r) && (this->g == b.g) && (this->b == b.b));   
    }
    
    bool RGBColor::operator!=(const RGBColor& b) const
    {
        return ((this->r != b.r) || (this->g != b.g) || (this->b != b.b));
    }

    RGBColor RGBColor::clamp() const
    {
        RGBColor b;
        b.r = std::max(0.0f, std::min(this->r, 1.0f));
        b.g = std::max(0.0f, std::min(this->g, 1.0f));
        b.b = std::max(0.0f, std::min(this->b, 1.0f));
        return b;
    }
    
    RGBColor RGBColor::gamma(float gam) const
    {
        return RGBColor();
    }
    
    float RGBColor::luminance() const
    {
        return 0;
    }

    RGBColor operator*(float scalar, const RGBColor& b)
    {
         return RGBColor(b.r*scalar, b.g*scalar, b.b*scalar);
    }
    
    RGBColor operator*(const RGBColor& a, float scalar)
    {
        return RGBColor(a.r*scalar, a.g*scalar, a.b*scalar);
    }
    
    RGBColor operator/(const RGBColor& a, float scalar)
    {
        return RGBColor(a.r/scalar, a.g/scalar, a.b/scalar);
    }

    RGBColor interp(const RGBColor& c1, const RGBColor& c2,
                    float dist)
    {
        RGBColor c;
        c.r = ((1-dist)* c1.r) + (dist * c2.r);
        c.g = ((1-dist)* c1.g) + (dist * c2.g);
        c.b = ((1-dist)* c1.b) + (dist * c2.b);
        return c;
    }
}
