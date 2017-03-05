#include <rt/groups/kdtree.h>
#include <vector>
#include <rt/intersection.h>
#include <cstring>
namespace rt
{
    KDTree::KDTree(const Primitives &primitives,
        int icost, int traversecost, float ebonus, int maxprimitives,
        int maxdepth) :
        isectCost(icost), traversalCost(traversecost), maxPrims(maxprimitives), maxDepth(maxdepth),
        emptyBonus(ebonus), bounds(BBox::empty()), SAH_HEURISTIC(false)
    {
        rebuildIndex();
    }

    void KDTree::rebuildIndex()
    {
            nextFreeNode = nAllocedNodes = 0;
            if (maxDepth <= 0)
                maxDepth = std::round(8 + 1.3f * std::log(float(primitives.size())));

            // Compute bounds for kd-tree construction
            std::vector<BBox> primBounds;
            primBounds.reserve(primitives.size());
            for (uint32_t i = 0; i < primitives.size(); ++i)
            {
                BBox b = primitives[i]->getBounds(); // worldbound
                bounds = bounds + b; // Union
                primBounds.push_back(b);
            }

            // Allocate working memory for kd-tree construction
            BoundEdge *edges[3];
            for (int i = 0; i < 3; ++i)
                edges[i] = new BoundEdge[2 * primitives.size()];

            uint32_t *prims0 = new uint32_t[primitives.size()];
            uint32_t *prims1 = new uint32_t[(maxDepth + 1) * primitives.size()];

            // Initialize _primNums_ for kd-tree construction
            uint32_t *primNums = new uint32_t[primitives.size()];
            for (uint32_t i = 0; i < primitives.size(); ++i)
                primNums[i] = i;

            // Start recursive construction of kd-tree
            if (SAH_HEURISTIC)
                buildTree(0, bounds, primBounds, primNums, primitives.size(), maxDepth, edges, prims0, prims1);
            else
                buildTreeNormal(0, bounds, primBounds, primNums, primitives.size(), maxDepth, edges, prims0, prims1,false);

            //Free working memory for kd - tree construction
            delete[] primNums;
            for (int i = 0; i < 3; ++i)
                delete[] edges[i];
            delete[] prims0;
            delete[] prims1;
    }
    
    void KDTree::add(Primitive* p)
    {
        primitives.push_back(p);
    }

    void KDTree::buildTree(int nodeNum, const BBox &nodeBounds,
        const std::vector<BBox> &allPrimBounds, uint32_t *primNums,
        int nPrimitives, int depth, BoundEdge *edges[3],
        uint32_t *prims0, uint32_t *prims1, int badRefines)
    {
        Assert(nodeNum == nextFreeNode);
        // Get next free node from _nodes_ array
        if (nextFreeNode == nAllocedNodes) {
            int nAlloc = max(2 * nAllocedNodes, 512);
            KdAccelNode *n = AllocAligned<KdAccelNode>(nAlloc);
            if (nAllocedNodes > 0) {
                memcpy(n, nodes, nAllocedNodes * sizeof(KdAccelNode));
                FreeAligned(nodes);
            }
            nodes = n;
            nAllocedNodes = nAlloc;
        }
        ++nextFreeNode;

        // Initialize leaf node if termination criteria met
        if (nPrimitives <= maxPrims || depth == 0) {
            nodes[nodeNum].initLeaf(primNums, nPrimitives, arena);
            return;
        }

        // Initialize interior node and continue recursion

        // Choose split axis position for interior node
        int bestAxis = -1, bestOffset = -1;
        float bestCost = INFINITY;
        float oldCost = isectCost * float(nPrimitives);
        float totalSA = nodeBounds.SurfaceArea();
        float invTotalSA = 1.f / totalSA;
        Vector d = nodeBounds.max - nodeBounds.min;

        // Choose which axis to split along
        uint32_t axis = nodeBounds.MaximumExtent();
        int retries = 0;
    retrySplit:

        // Initialize edges for _axis_
        for (int i = 0; i < nPrimitives; ++i) {
            int pn = primNums[i];
            const BBox &bbox = allPrimBounds[pn];
            edges[axis][2 * i] = BoundEdge(bbox.min[axis], pn, true);
            edges[axis][2 * i + 1] = BoundEdge(bbox.max[axis], pn, false);
        }
        std::sort(&edges[axis][0], &edges[axis][2 * nPrimitives]);

        // Compute cost of all splits for _axis_ to find best
        int nBelow = 0, nAbove = nPrimitives;
        for (int i = 0; i < 2 * nPrimitives; ++i) {
            if (edges[axis][i].type == BoundEdge::END) --nAbove;
            float edget = edges[axis][i].t;
            if (edget > nodeBounds.min[axis] &&
                edget < nodeBounds.max[axis]) {
                // Compute cost for split at _i_th edge
                uint32_t otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
                float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                    (edget - nodeBounds.min[axis]) *
                    (d[otherAxis0] + d[otherAxis1]));
                float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                    (nodeBounds.max[axis] - edget) *
                    (d[otherAxis0] + d[otherAxis1]));
                float pBelow = belowSA * invTotalSA;
                float pAbove = aboveSA * invTotalSA;
                float eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0.f;
                float cost = traversalCost +
                    isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);

                // Update best split if this is lowest cost so far
                if (cost < bestCost)  {
                    bestCost = cost;
                    bestAxis = axis;
                    bestOffset = i;
                }
            }
            if (edges[axis][i].type == BoundEdge::START) ++nBelow;
        }
        Assert(nBelow == nPrimitives && nAbove == 0);

        // Create leaf if no good splits were found
        if (bestAxis == -1 && retries < 2) {
            ++retries;
            axis = (axis + 1) % 3;
            goto retrySplit;
        }
        if (bestCost > oldCost) ++badRefines;
        if ((bestCost > 4.f * oldCost && nPrimitives < 16) ||
            bestAxis == -1 || badRefines == 3) {
            nodes[nodeNum].initLeaf(primNums, nPrimitives, arena);
            return;
        }

        // Classify primitives with respect to split
        int n0 = 0, n1 = 0;
        for (int i = 0; i < bestOffset; ++i)
            if (edges[bestAxis][i].type == BoundEdge::START)
                prims0[n0++] = edges[bestAxis][i].primNum;
        for (int i = bestOffset + 1; i < 2 * nPrimitives; ++i)
            if (edges[bestAxis][i].type == BoundEdge::END)
                prims1[n1++] = edges[bestAxis][i].primNum;

        // Recursively initialize children nodes
        float tsplit = edges[bestAxis][bestOffset].t;
        BBox bounds0 = nodeBounds, bounds1 = nodeBounds;
        bounds0.SetMax(bestAxis, tsplit);
        bounds1.SetMin(bestAxis, tsplit);
        buildTree(nodeNum + 1, bounds0,
            allPrimBounds, prims0, n0, depth - 1, edges,
            prims0, prims1 + nPrimitives, badRefines);
        uint32_t aboveChild = nextFreeNode;
        nodes[nodeNum].initInterior(bestAxis, aboveChild, tsplit);
        buildTree(aboveChild, bounds1, allPrimBounds, prims1, n1,
            depth - 1, edges, prims0, prims1 + nPrimitives, badRefines);
    }

    void KDTree::buildTreeNormal(int nodeNum, const BBox &nodeBounds,
        const std::vector<BBox> &allPrimBounds, uint32_t *primNums,
        int nPrimitives, int depth, BoundEdge *edges[3],
        uint32_t *prims0, uint32_t *prims1, bool isLeaf, int badRefines)
    {
        Assert(nodeNum == nextFreeNode);
        // Get next free node from _nodes_ array
        if (nextFreeNode == nAllocedNodes) {
            int nAlloc = max(2 * nAllocedNodes, 512);
            KdAccelNode *n = AllocAligned<KdAccelNode>(nAlloc);
            if (nAllocedNodes > 0) {
                
                memcpy(n, nodes, nAllocedNodes * sizeof(KdAccelNode));
                FreeAligned(nodes);
            }
            nodes = n;
            nAllocedNodes = nAlloc;
        }
        ++nextFreeNode;

        // Initialize leaf node if termination criteria met
        if (nPrimitives <= maxPrims || depth == 0 || isLeaf) {
            nodes[nodeNum].initLeaf(primNums, nPrimitives, arena);
            return;
        }

        // Initialize interior node and continue recursion

        // Choose split axis position for interior node

        // Choose which axis to split along
        uint32_t axis = nodeBounds.MaximumExtent();
        float tsplit = (nodeBounds.max[axis] + nodeBounds.min[axis]) / 2;

        // Initialize edges for _axis_
        for (int i = 0; i < nPrimitives; ++i) {
            int pn = primNums[i];
            const BBox &bbox = allPrimBounds[pn];
            edges[axis][2 * i] = BoundEdge(bbox.min[axis], pn, true);
            edges[axis][2 * i + 1] = BoundEdge(bbox.max[axis], pn, false);
        }
        std::sort(&edges[axis][0], &edges[axis][2 * nPrimitives]);

        // Classify primitives with respect to split
        int n0 = 0, n1 = 0;
        for (int i = 0; i < 2 * nPrimitives; ++i)
        {
            if (edges[axis][i].t < tsplit && edges[axis][i].type == BoundEdge::START)
                prims0[n0++] = edges[axis][i].primNum;
            if (edges[axis][i].t >= tsplit && edges[axis][i].type == BoundEdge::END)
                prims1[n1++] = edges[axis][i].primNum;
        }

        // Recursively initialize children nodes
        BBox bounds0 = nodeBounds, bounds1 = nodeBounds;
        bounds0.SetMax(axis, tsplit);
        bounds1.SetMin(axis, tsplit);
        
        buildTreeNormal(nodeNum + 1, bounds0,
            allPrimBounds, prims0, n0, depth - 1, edges,
            prims0, prims1 + nPrimitives, false, badRefines);

        uint32_t aboveChild = nextFreeNode;
        nodes[nodeNum].initInterior(axis, aboveChild, tsplit);

        buildTreeNormal(aboveChild, bounds1, allPrimBounds, prims1, n1,
            depth - 1, edges, prims0, prims1 + nPrimitives, false, badRefines);

    }

    void KdAccelNode::initLeaf(uint32_t *primNums, int np, MemoryArena &arena)
    {
        this->flags = 3;
        nPrims |= (np << 2);

        // Store primitive ids for leaf node
        if (np == 0)
            onePrimitive = 0;
        else if (np == 1)
            onePrimitive = primNums[0];
        else {
            primitives = arena.Alloc<uint32_t>(np);
            for (int i = 0; i < np; ++i)
                primitives[i] = primNums[i];
        }
    }

    void KdAccelNode::initInterior(uint32_t axis, uint32_t ac, float s) {
        split = s;
        flags = axis;
        aboveChild |= (ac << 2);
    }

    Intersection KDTree::intersect(const Ray& ray, float previousBestDistance) const
    {
        //return Intersection::failure();
        // Compute initial parametric range of ray inside kd-tree extent
        float tmin, tmax;
        std::pair<float, float> maxmin = bounds.intersect(ray);
        tmax = maxmin.second;
        tmin = maxmin.first;
        if (tmax < tmin)
        {
            return Intersection::failure();
        }

        // Prepare to traverse kd-tree for ray
        Vector invDir(1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z);
#define MAX_TODO 64
        KdToDo todo[MAX_TODO];
        int todoPos = 0;
        // Traverse kd-tree nodes in order for ray
        bool hit = false;
        Intersection intersect = Intersection::failure();
        const KdAccelNode *node = &nodes[0];
        while (node != NULL) {
            // Bail out if we found a hit closer than the current node
            if (previousBestDistance < tmin) break;
            if (!node->IsLeaf()) {
                // Process kd-tree interior node

                // Compute parametric distance along ray to split plane
                int axis = node->SplitAxis();
                float tplane = (node->SplitPos() - ray.o[axis]) * invDir[axis];

                // Get node children pointers for ray
                const KdAccelNode *firstChild, *secondChild;
                int belowFirst = (ray.o[axis] < node->SplitPos()) ||
                    (ray.o[axis] == node->SplitPos() && ray.d[axis] <= 0);
                if (belowFirst) {
                    firstChild = node + 1;
                    secondChild = &nodes[node->AboveChild()];
                }
                else {
                    firstChild = &nodes[node->AboveChild()];
                    secondChild = node + 1;
                }

                // Advance to next child node, possibly enqueue other child
                if (tplane > tmax || tplane <= 0)
                    node = firstChild;
                else if (tplane < tmin)
                    node = secondChild;
                else {
                    // Enqueue _secondChild_ in todo list
                    todo[todoPos].node = secondChild;
                    todo[todoPos].tmin = tplane;
                    todo[todoPos].tmax = tmax;
                    ++todoPos;
                    node = firstChild;
                    tmax = tplane;
                }
            }
            else 
            {
                // Check for intersections inside leaf node
                uint32_t nPrimitives = node->nPrimitives();
                if (nPrimitives == 1)
                {
                    Primitive *prim = primitives[node->onePrimitive];
                    float minDist = FLT_MAX;
                    Intersection tmpIntersect = Intersection::failure();
                    // Check one primitive inside leaf node
                    tmpIntersect = prim->intersect(ray);
                    if (tmpIntersect && tmpIntersect.distance < previousBestDistance)
                    {
                        intersect = tmpIntersect;
                        minDist = tmpIntersect.distance;
                        previousBestDistance = minDist;
                    }
                }
                else 
                {
                    uint32_t *prims = node->primitives;
                    float minDist = FLT_MAX;
                    Intersection tmpIntersect = Intersection::failure();
                    for (uint32_t i = 0; i < nPrimitives; ++i) {
                        Primitive *prim = primitives[prims[i]];
                        // Check one primitive inside leaf node
                        tmpIntersect = prim->intersect(ray);
                        if (tmpIntersect && tmpIntersect.distance < previousBestDistance && tmpIntersect.distance < minDist)
                        {
                            intersect = tmpIntersect;
                            minDist = tmpIntersect.distance;
                            previousBestDistance = minDist;
                        }
                    }
                }

                // Grab next node to process from todo list
                if (todoPos > 0) {
                    --todoPos;
                    node = todo[todoPos].node;
                    tmin = todo[todoPos].tmin;
                    tmax = todo[todoPos].tmax;
                }
                else
                    break;
            }
        }
        return intersect;
    }

}
