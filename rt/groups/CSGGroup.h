#pragma once
#include <rt/groups/group.h>
#include <rt/intersection.h>
#include <rt/bbox.h>
#include <assert.h>

namespace rt
{

    enum BooleanOperation { UNION, INTERSECTION, SUBTRACTION};
    class CSGGroup : public Group
    {
    public:
        CSGGroup(Primitive* left, Primitive* right, BooleanOperation operation) :
            left(left), right(right), operation(operation),box(left->getBounds() + right->getBounds()){
            Assert((right != nullptr && left != nullptr) || (right == nullptr && left == nullptr)) << "Neither leaf or node";//whether leaf or node
            
        }

        virtual Intersection intersect(const Ray& ray, float previousBestDistance = FLT_MAX) const;
        std::vector<Intersection> intersectChildren(const Ray& ray) const;
        virtual void rebuildIndex() {}
        virtual void add(Primitive* p) { box.extend(p->getBounds()); }
        virtual BBox getBounds() const{ return box; }
        virtual void setMaterial(Material* m){}
        virtual void setCoordMapper(CoordMapper* cm){}
    private:
        Primitive *left, *right;//must be necessarily CSGGroup or solid
        BooleanOperation operation;
        BBox box;


    };
}