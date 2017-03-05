#include <rt/primmod/bmap.h>

namespace rt
{
    BumpMapper::BumpMapper(Triangle* base, Texture* bumpmap,
                           const Point& bv1, const Point& bv2,
                           const Point& bv3, float vscale) :
            base(base), bumpmap(bumpmap), bv1(bv1), bv2(bv2),
            bv3(bv3), vscale(vscale)
    {
        
    }

    BBox BumpMapper::getBounds() const
    {
        return base->getBounds();
    }

    Intersection BumpMapper::intersect(
        const Ray& ray, float previousBestDistance) const
    {
        Intersection result = base->intersect(ray, previousBestDistance);

        if(result)
        {
            Vector dpdu, dpdv;
            float uvs[3][2]; //points in uv coords
            uvs[0][0] = bv1[0];
            uvs[0][1] = bv1[1];
            uvs[1][0] = bv2[0];
            uvs[1][1] = bv2[1];
            uvs[2][0] = bv3[0];
            uvs[2][1] = bv3[1];
            
            //float du1 = uvs[0][0] - uvs[2][0];
            //float du2 = uvs[1][0] - uvs[2][0];
            //float dv1 = uvs[0][1] - uvs[2][1];
            //float dv2 = uvs[1][1] - uvs[2][1];

            float du1 = bv1[0] - bv3[0];
            float du2 = bv2[0] - bv3[0];
            float dv1 = bv1[1] - bv3[1];
            float dv2 = bv2[1] - bv3[1];
            
            Vector dp1 = base->vertices[0] - base->vertices[2];
            Vector dp2 = base->vertices[1] - base->vertices[2];

            float determinant = du1 * dv2 - dv1 * du2;

            Assert(determinant != 0.0f);
            
            float invdet = 1.0f / determinant;
            
            dpdu = (dv2*dp1 - dv1*dp2) * invdet;
            dpdv = (-du2*dp1 + du1*dp2) * invdet;

            Vector nn = cross(dpdu, dpdv).normalize();
            
            Point uv = result.uv;
            float u = uv[0]*uvs[0][0] + uv[1]*uvs[1][0] + uv[2]*uvs[2][0];
            float v = uv[0]*uvs[0][1] + uv[1]*uvs[1][1] + uv[2]*uvs[2][1];
            
            RGBColor dx = bumpmap->getColorDX(Point(u, v, 0));
            RGBColor dy = bumpmap->getColorDY(Point(u, v, 0));

            float vvscale = vscale * 0.3f;
            dpdu = dpdu + vscale * (dx.r) * nn;
            dpdv = dpdv + vscale * (dy.r) * nn;

            //Vector new_norm = cross(dpdu, dpdv).normalize();
            Vector new_norm = nn + vscale * dx.r * cross(nn, dpdv)
                + vscale * dy.r * cross(nn, dpdu);
            
            if(rt::dot(new_norm, nn) < 0.0f)
            {
                new_norm = -1.0f * new_norm;
            }

            result.norm = new_norm.normalize();
        }
        
        return result;
    }

    void BumpMapper::setMaterial(Material* m)
    {
        base->setMaterial(m);
    }

    void BumpMapper::setCoordMapper(CoordMapper* cm)
    {
        base->setCoordMapper(cm);
    }
};
