#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"
#include "interval.h"
#include <cmath>

//This class is a bounding box, it is defined by a min and max point

class aabb {
    public:
    aabb(const point3& aabbMin, const point3& aabbMax) : aabbMin(aabbMin), aabbMax(aabbMax) {

    }

    //Define how to hit the bounding box
    //Is basically working out where each ray enters and exits the aabb and then checks if the ray is in the box in all coordinates at 1 point in time, therefore hit

    bool hit(const ray& r, interval ray_t) const {
        //X-axis
        auto tx0 = (aabbMin.x() - r.origin().x()) / r.direction().x();
        auto tx1 = (aabbMax.x() - r.origin().x()) / r.direction().x();
        auto entryX = std::fmin(tx0,tx1);
        auto exitX = std::fmax(tx0,tx1);

        //Y-axis
        auto ty0 = (aabbMin.y() - r.origin().y()) / r.direction().y();
        auto ty1 = (aabbMax.y() - r.origin().y()) / r.direction().y();
        auto entryY = std::fmin(ty0,ty1);
        auto exitY = std::fmax(ty0,ty1);

        //Z-axis
        auto tz0 = (aabbMin.z() - r.origin().z()) / r.direction().z();
        auto tz1 = (aabbMax.z() - r.origin().z()) / r.direction().z();
        auto entryZ = std::fmin(tz0,tz1);
        auto exitZ = std::fmax(tz0,tz1);

        //Is the last entry point before the first exit from an axis?
        //If so then the ray hit (also checks ray_t for behind camera shenanigans)
        auto lastEntry = std::fmax(entryX, std::fmax(entryY, std::fmax(ray_t.min, entryZ)));
        auto firstExit = std::fmin(exitX, std::fmin(exitY, std::fmin(ray_t.max, exitZ)));

        if (lastEntry <= firstExit) {
            return true;
        }
        return false;
        
    }

    point3 center() const {
        return (aabbMin + aabbMax) * 0.5;
    }
    //Getters
    const point3& min() const { return aabbMin; }
    const point3& max() const { return aabbMax; }

    private:
    point3 aabbMin, aabbMax;

};


#endif