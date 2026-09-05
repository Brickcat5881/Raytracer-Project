#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "hittable.h"

//Used to move hittable objects in the scene
//Moves the rays instead of the object which is easier
class translate: public hittable {
    public:
    translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset) {} 

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        ray offset_r(r.origin() - offset, r.direction());
        if (!object->hit(offset_r, ray_t, rec)) {
            return false;
        }
        rec.p += offset;
        return true;
    }

    aabb bounding_box() const override {
        aabb originalBB = object->bounding_box();
        aabb offsetBB = aabb(originalBB.min()+offset,originalBB.max()+offset);
        return offsetBB;
    }
    private:
    shared_ptr<hittable> object;
    vec3 offset;
};

#endif