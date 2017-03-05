#include <rt/textures/perlin.h>
#include <core/point.h>
#include <core/interpolate.h>
#include <core/scalar.h>

namespace rt {

    namespace {

        /* returns a value in range -1 to 1 */
        float noise(int x, int y, int z) {
            int n = x + y * 57 + z * 997;
            n = (n << 13) ^ n;
            return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
        }

    }

    PerlinTexture::PerlinTexture(const RGBColor& white, const RGBColor& black) :white(white), black(black)
    {

    }

    void PerlinTexture::addOctave(float amplitude, float frequency)
    {
        amplitudes.push_back(amplitude);
        frequencies.push_back(frequency);
    }

    float PerlinTexture::SmoothedNoise(float x, float y, float z)
    {
        float corners = (noise(x - 1, y - 1, z - 1) + noise(x - 1, y - 1, z + 1) +
            noise(x + 1, y - 1, z - 1) + noise(x + 1, y - 1, z + 1) +
            noise(x - 1, y + 1, z - 1) + noise(x - 1, y + 1, z + 1) +
            noise(x + 1, y + 1, z - 1) + noise(x + 1, y + 1, z + 1)) / 32;
        float sides = (noise(x - 1, y, z) + noise(x + 1, y, z) +
            noise(x, y - 1, z) + noise(x, y + 1, z) +
            noise(x, y, z - 1) + noise(x, y, z + 1)) / 12;
        float center = noise(x, y, z) / 8;
        return corners + sides + center;
        
    }
    
    float PerlinTexture::InterpolatedNoise(float x, float y, float z)
    {
        int intx = floor(x);
        float fltx = x - intx;

        int inty = floor(y);
        float flty = y - inty;

        int intz = floor(z);
        float fltz = z - intz;

        float v1 = noise(intx,     inty,     intz);
        float v2 = noise(intx + 1, inty,     intz);
        float v3 = noise(intx,     inty + 1, intz);
        float v4 = noise(intx + 1, inty + 1, intz);
        float v5 = noise(intx,     inty,     intz + 1);
        float v6 = noise(intx + 1, inty,     intz + 1);
        float v7 = noise(intx,     inty + 1, intz + 1);
        float v8 = noise(intx + 1, inty + 1, intz + 1);

        float i1 = lerp(v1, v2, fltx);
        float i2 = lerp(v3, v4, fltx);
        float i3 = lerp(v5, v6, fltx);
        float i4 = lerp(v7, v8, fltx);

        float d1 = lerp(i1, i2, flty);
        float d2 = lerp(i3, i4, flty);

        return lerp(d1, d2, fltz);
    }

    RGBColor PerlinTexture::getColor(const Point& coord)
    {
        float total = 0;
        for (int i = 0; i < amplitudes.size(); ++i)
        {
            total += amplitudes[i] * InterpolatedNoise(coord.x*frequencies[i], coord.y*frequencies[i], coord.z*frequencies[i]);
        }

        return (white - black) * total + black;
        //return white * total;
    }

    RGBColor PerlinTexture::getColorDX(const Point& coord)
    {
        NOT_IMPLEMENTED;
    }

    RGBColor PerlinTexture::getColorDY(const Point& coord)
    {
        NOT_IMPLEMENTED;
    }


}