#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"


#include <vector>

//A list of hittable objects that is itself a hittable object
class hittable_list : public hittable {
    public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) {add(object);}

    void clear() {objects.clear();}

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }


    //Basic hit function to find which item in the hittable list has been hit and is infront
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }

    //Every Hittable needs a bounding box, this one contains everything so is always hit
    aabb bounding_box() const override {
        point3 bmin(1e30, 1e30, 1e30);
        point3 bmax(-1e30, -1e30, -1e30);
        for (const auto& object : objects) {
            aabb box = object->bounding_box();
            bmin = point3(std::fmin(bmin.x(), box.min().x()), std::fmin(bmin.y(), box.min().y()), std::fmin(bmin.z(), box.min().z()));
            bmax = point3(std::fmax(bmax.x(), box.max().x()), std::fmax(bmax.y(), box.max().y()), std::fmax(bmax.z(), box.max().z()));
        }
        return aabb(bmin, bmax);
    }
};

#endif