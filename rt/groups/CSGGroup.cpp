#include <rt/groups/CSGGroup.h>
#include <core/assert.h>

namespace rt
{

    Intersection CSGGroup::intersect(const Ray& ray, float previousBestDistance) const
    {
        Assert(left != nullptr && right != nullptr) << "CSGGroup node children not present\n";
        std::vector<Intersection> totalIntersections = intersectChildren(ray);
        float mindist = FLT_MAX;
        Intersection intersection = Intersection::failure();
        for (auto inter : totalIntersections)
        {
            if (inter.distance < mindist)
            {
                intersection = inter;
                mindist = intersection.distance;
            }
        }
        return intersection;
    }

    std::vector<Intersection> CSGGroup::intersectChildren(const Ray& ray) const
    {
        if (left == nullptr || right == nullptr)
            return std::vector<Intersection>();
        
        CSGGroup* leftGroup = dynamic_cast<CSGGroup*> (left);
        CSGGroup* rightGroup = dynamic_cast<CSGGroup*> (right);
        std::vector<Intersection> leftIntersections, rightIntersections;
        std::vector<Intersection> totalIntersections;
        if (leftGroup)
        {
            leftIntersections = leftGroup->intersectChildren(ray);
        }
        else
        {
            Intersection inter = left->intersect(ray);
            if (inter)
                leftIntersections.push_back(inter);
        }

        if (rightGroup)
        {
            rightIntersections = rightGroup->intersectChildren(ray);
        }
        else
        {
            Intersection inter = right->intersect(ray);
            if (inter)
                rightIntersections.push_back(inter);
        }

        if (operation == UNION)
        {
            if (leftIntersections.size() == 0)
                totalIntersections = rightIntersections;
            else if (rightIntersections.size() == 0)
                totalIntersections = leftIntersections;

            for  (auto leftInter : leftIntersections)
            {
                for  (auto rightInter : rightIntersections)
                {
                    if (operation == UNION)
                    {
                        std::vector<Intersection> inters = leftInter | rightInter;
                        totalIntersections.insert(totalIntersections.begin(), inters.begin(), inters.end());
                    }
                }
            }
        }
        else if (operation == SUBTRACTION)
        {
            totalIntersections = leftIntersections;
            for (int i = 0; i < totalIntersections.size();)
            {
                for  (auto rightInter : rightIntersections)
                {
                    if (operation == SUBTRACTION)
                    {
                        std::vector<Intersection> tmp = totalIntersections[i] - rightInter;
                        if (tmp.size())
                        {
                            totalIntersections.erase(totalIntersections.begin() + i);
                            totalIntersections.insert(totalIntersections.begin(), tmp.begin(), tmp.end());
                        }
                        else
                            ++i;
                    }
                }
                ++i;
            }
        }
        else // Intersection
        {
            for  (auto leftInter : leftIntersections)
            {
                for  (auto rightInter : rightIntersections)
                {
                    Intersection inter = leftInter & rightInter;
                    if (inter)
                        totalIntersections.push_back(inter);

                }
            }
        }
        return totalIntersections;
    }
}
