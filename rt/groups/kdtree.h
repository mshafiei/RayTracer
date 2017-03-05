#ifndef CG1RAYTRACER_GROUPS_KDTREE_HEADER
#define CG1RAYTRACER_GROUPS_KDTREE_HEADER

#include <vector>
#include <core/memoryarena.h>
#include <rt/groups/group.h>
#include <rt/bbox.h>

namespace rt {
    //KDTree Acceleration according to the PBR book pg. 227
    
    class KdAccelNode {
    public:
        void initLeaf(uint32_t *primNums, int np, MemoryArena &arena);
        void initInterior(uint32_t axis, uint32_t ac, float s);
        float SplitPos() const { return split; }
        uint32_t nPrimitives() const { return nPrims >> 2; }
        uint32_t SplitAxis() const { return flags & 3; }
        bool IsLeaf() const { return (flags & 3) == 3; }
        uint32_t AboveChild() const { return aboveChild >> 2; }
        union {
            float split; // Interior
            uint32_t onePrimitive; // Leaf
            uint32_t *primitives; // Leaf
        };

    private:
        union {
            uint32_t flags; // Both, 0 = x, 1 = y, 2 = z, 3 = leaf
            uint32_t nPrims; // Leaf, how many primitives overlaps this node
            uint32_t aboveChild; // Interior
        };
    };

    class KdToDo {
    public:
        const KdAccelNode *node;
        float tmin, tmax;
    };

    class KDTree : public Group {
        struct BoundEdge {
        public:
            BoundEdge() { }
            BoundEdge(float tt, int pn, bool starting) {
                t = tt;
                primNum = pn;
                type = starting ? START : END;
            }
            bool operator<(const BoundEdge &e) const {
                if (t == e.t)
                    return (int)type < (int)e.type;
                else return t < e.t;
            }
        
            float t;
            int primNum;
            enum { START, END } type;
        };

        public:
            KDTree(const Primitives &primitives,
                int icost, int traversecost, float ebonus, int maxprimitives,
                int maxdepth);
            virtual BBox getBounds() const{ return bounds; }
            virtual Intersection intersect(const Ray& ray, float previousBestDistance = FLT_MAX) const;
            virtual void rebuildIndex();
            virtual ~KDTree(){}
            virtual void add(Primitive* p);
            virtual void setMaterial(Material* m){ NOT_IMPLEMENTED; }
            virtual void setCoordMapper(CoordMapper* cm){ NOT_IMPLEMENTED; }
            float SAH(const BBox &nodeBounds, uint32_t *primNums,
                int nPrimitives, const std::vector<BBox> &allPrimBounds, BoundEdge *edges[3],
                uint32_t *prims0, uint32_t *prims1, int badRefines, int nodeNum, uint32_t& bestAxis);
            float CenterAxisHeuristic(const BBox &nodeBounds, uint32_t& bestAxis);
            int isectCost, traversalCost, maxPrims, maxDepth;
            float emptyBonus;

    private:
        void buildTree(int nodeNum, const BBox &nodeBounds,
            const std::vector<BBox> &allPrimBounds, uint32_t *primNums,
            int nPrimitives, int depth, BoundEdge *edges[3],
            uint32_t *prims0, uint32_t *prims1, int badRefines = 0);

        void buildTreeNormal(int nodeNum, const BBox &nodeBounds,
            const std::vector<BBox> &allPrimBounds, uint32_t *primNums,
            int nPrimitives, int depth, BoundEdge *edges[3],
            uint32_t *prims0, uint32_t *prims1, bool isLeaf, int badRefines = 0);

        KdAccelNode *nodes;
        int nAllocedNodes, nextFreeNode;
        BBox bounds;
        MemoryArena arena;
        const bool SAH_HEURISTIC;
        };

    }

#endif
