#include <rt/groups/simplegroup.h>
#include <core/assert.h>
#include <rt/bbox.h>
#include <rt/intersection.h>

namespace rt
{   
    BBox SimpleGroup::getBounds() const
    {
        return bounds;
    }
    void SimpleGroup::computeBounds()
    {
        bounds = BBox::empty();
        for (auto p : primitives)
        {
            bounds.extend(p->getBounds());
        }
    }

    Intersection SimpleGroup::intersect(const Ray& ray,
                                        float previousBestDistance) const
    {
        Intersection finalHit, hit;
        finalHit = Intersection::failure();
        
        for(auto p : primitives)
        {
            hit = p->intersect(ray, previousBestDistance);
            if(hit && hit.distance < finalHit.distance)
            {
                finalHit = hit;
                previousBestDistance = hit.distance;
            }
        }

        return finalHit;
    }

    void SimpleGroup::rebuildIndex()
    {
        // for SimpleGroup's no index rebuilding is needed since we
        // don't use any indexing structure
        computeBounds();
    }

    void SimpleGroup::add(Primitive* p)
    {
        primitives.push_back(p);
    }

    void SimpleGroup::setMaterial(Material* m)
    {
        for (auto p = primitives.begin(); p != primitives.end(); ++p)
        {
            (*p)->setMaterial(m);
        }
    }

    void SimpleGroup::setCoordMapper(CoordMapper* cm)
    {
        NOT_IMPLEMENTED;
    }
}
