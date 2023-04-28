#pragma once
namespace Entity
{
    struct Plane
    {
        glm::vec3 normal;
        glm::vec3 Po;
        Plane(glm::vec3 n, glm::vec3 p = glm::vec3(0)) : normal(n), Po(p)
        {}
    };


    class Octree
    {
    public:
        #define dateType animeModel*
        std::unordered_map<dateType, int> hasModel;
        glm::vec3 pointMin;
        glm::vec3 pointMax;
        std::vector<Octree> children;
        int layer;
        Octree(const Octree& o)
        {
            hasModel = o.hasModel;
            pointMax = o.pointMax;
            pointMin = o.pointMin;
            children = o.children;
        };
        Octree(int n, glm::vec3 pMin, glm::vec3 pMax) : layer(n), pointMin(pMin), pointMax(pMax)
        {
        }
        Octree(int n) : layer(n)
        {
            float min_size = float(1 << n);
            pointMin = glm::vec3(-min_size);
            pointMax = glm::vec3(min_size);
        }
        void update(dateType m)
        {
            searchRemove(m);
            updateModel(m);
        }
        bool updateModel(dateType m)
        {    
            if (inScen(m))
            {
                bornChildren(layer - 1);
                int i;
                for (i = 0; i < children.size(); i++)
                {
                    if (children[i].updateModel(m))
                    {
                        hasModel[m] = i;
                        break;
                    }
                }
                if (i == children.size())
                    hasModel[m] = 8;
                return true;
            }
            else
                return false;
        }
        bool searchRemove(dateType model)
        {
            if (hasModel.find(model) != hasModel.end())
            {
                if (hasModel[model] == 8)
                {
                    hasModel.erase(model);
                    if (hasModel.empty())
                        children.clear();
                }
                else
                {
                    children[hasModel[model]].searchRemove(model);
                    hasModel.erase(model);
                    if (hasModel.empty())
                        children.clear();
                }
                return true;
            }
            else
            {
                return false;
            }
        }
        bool inScen(Model* m)
        {
            std::vector<float> a = m->aabb;
            return
                a[min_x] >= pointMin.x &&
                a[min_y] >= pointMin.y &&
                a[min_z] >= pointMin.z &&
                a[max_x] <= pointMax.x &&
                a[max_y] <= pointMax.y &&
                a[max_z] <= pointMax.z;
        }

        void bornChildren(int n)
        {
            if (children.empty())
            {
                float min_size = float(1 << n);
                glm::vec3 center = (pointMin + pointMax) * 0.5f;
                Octree child_0(n, glm::vec3(pointMin.x, center.y, center.z), glm::vec3(center.x, pointMax.y, pointMax.z));
                children.push_back(child_0);

                Octree child_1(n, center, pointMax);
                children.push_back(child_1);

                Octree child_2(n, glm::vec3(pointMin.x, pointMin.y, center.z), glm::vec3(center.x, center.y, pointMax.z));
                children.push_back(child_2);

                Octree child_3(n, glm::vec3(center.x, pointMin.y, center.z), glm::vec3(pointMax.x, center.y, pointMax.z));
                children.push_back(child_3);

                Octree child_4(n, glm::vec3(pointMin.x, center.y, pointMin.z), glm::vec3(center.x, pointMax.y, center.z));
                children.push_back(child_4);

                Octree child_5(n, glm::vec3(center.x, center.y, pointMin.z), glm::vec3(pointMax.x, pointMax.y, center.z));
                children.push_back(child_5);

                Octree child_6(n, pointMin, center);
                children.push_back(child_6);

                Octree child_7(n, glm::vec3(center.x, pointMin.y, pointMin.z), glm::vec3(pointMax.x, center.y, center.z));
                children.push_back(child_7);
            }
        }
    };
}

