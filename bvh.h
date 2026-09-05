#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "vec3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "interval.h"
#include "aabb.h"
#include <vector>
#include <memory>

//Struct is a class with only public attributes, just a structure which is then used for each node in the tree
struct bvh_node {
    point3 aabbMin;
    point3 aabbMax;
    int leftChild;
    int rightChild;
    bool isLeaf;
    int firstPrim;
    int primCount;
};

//Class which defines the BVH, inherits hittable as it is a hittable object which contains the original object
class bvh : public hittable {
    public:
    bvh(hittable_list list) {
        objects = list.objects;
        build_bvh();
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        return intersect_node(rootNodeIdx, r, ray_t, rec); //Start traversing from the root
    }

    aabb bounding_box() const override {
    return aabb(nodes[rootNodeIdx].aabbMin, nodes[rootNodeIdx].aabbMax);
    }   

    private:
    std::vector<bvh_node> nodes; //A vector of nodes, as defined in the struct. 
    std::vector<shared_ptr<hittable>> objects; //A vector of hittable objects
    int rootNodeIdx = 0;
    int nodesUsed = 1;

    void build_bvh() {
        int N = objects.size();
        nodes.resize(N * 2);

        //Assigns all the triangles to the root
        bvh_node& root = nodes[rootNodeIdx];
        root.leftChild = 0;
        root.rightChild = 0;
        root.firstPrim = 0;
        root.primCount = N;
        
        update_node_bounds(rootNodeIdx);

        subdivide(rootNodeIdx);
    }

    void update_node_bounds(int nodeIdx) {
        bvh_node& node = nodes[nodeIdx];
        node.aabbMin = point3(1e30, 1e30, 1e30);
        node.aabbMax = point3(-1e30, -1e30, -1e30);

        //Loops through each object and finds the min and max coords to define the outer BVH for the root
        for (int i = 0; i < node.primCount; i++) {
            auto& obj = objects[node.firstPrim + i];
            aabb box = obj->bounding_box(); // Get the object's box!
            
            // Expand the node's box to enclose this object:
            node.aabbMin = point3(
            std::fmin(node.aabbMin.x(), box.min().x()),
            std::fmin(node.aabbMin.y(), box.min().y()),
            std::fmin(node.aabbMin.z(), box.min().z())
            );
            node.aabbMax = point3(
            std::fmax(node.aabbMax.x(), box.max().x()),
            std::fmax(node.aabbMax.y(), box.max().y()),
            std::fmax(node.aabbMax.z(), box.max().z())
            );
        }
    }

    void subdivide(int nodeIdx) {
        bvh_node& node = nodes[nodeIdx];
        if (node.primCount <= 2) return; //Stopping Condition

        //Determining split axis
        auto extent = node.aabbMax - node.aabbMin;
        int axis = 0;
        if (extent.y () > extent.x()) axis = 1;
        if (extent.z() > extent[axis]) axis = 2;
        auto splitPos = node.aabbMin[axis] + extent[axis] * 0.5;

        int i = node.firstPrim;
        int j = i + node.primCount - 1;
        while (i <= j) {
            if (objects[i]->bounding_box().center()[axis] < splitPos) i++;
            else std::swap(objects[i], objects[j--]);
        }

        //If 1 side is empty then return
        int leftCount = i - node.firstPrim;
        if (leftCount == 0 || leftCount == node.primCount) return;
        
        //Creating the child nodes
        int leftChildIndex = nodesUsed++;
        int rightChildIndex = nodesUsed++;
        node.leftChild = leftChildIndex;
        node.rightChild = rightChildIndex;

        nodes[leftChildIndex].firstPrim = node.firstPrim;
        nodes[leftChildIndex].primCount = leftCount;

        nodes[rightChildIndex].firstPrim = i;
        nodes[rightChildIndex].primCount = node.primCount - leftCount;
        node.primCount = 0;

        //Create new AABBs for each child
        update_node_bounds(leftChildIndex);
        update_node_bounds(rightChildIndex);

        //Recursion YAY
        subdivide(leftChildIndex);
        subdivide(rightChildIndex);

    }


    //Called in hit()
    bool intersect_node(int nodeIdx, const ray& r, interval ray_t, hit_record& rec) const {
        const bvh_node& node = nodes[nodeIdx];

        if (!aabb(node.aabbMin, node.aabbMax).hit(r, ray_t)) {
            return false;
        }

        //STOPPING CONDITION. Iterates through the prims and finds the closest one that is hit
        if (node.primCount > 0) {
            bool hit_anything = false;
            auto closest_so_far = ray_t.max;

            for (int i = 0; i < node.primCount; i++) {
                int obj_index = node.firstPrim + i;

                if (objects[obj_index]->hit(r, interval(ray_t.min, closest_so_far), rec)) {
                    hit_anything = true;
                    closest_so_far = rec.t;
                }
            }
            return hit_anything;
        }

        //RECURSE. Recurses when the intersected node is not a leaf and continues through the tree
        else {
            bool hit_left = intersect_node(node.leftChild, r, ray_t, rec);
            interval right_interval = hit_left ? interval(ray_t.min, rec.t) : ray_t;
            bool hit_right = intersect_node(node.rightChild, r, right_interval, rec);
            return hit_left || hit_right;
        }
        
    
    }
    
    
};
    



#endif