#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "hittable.h"

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
    private:
    shared_ptr<hittable> object;
    vec3 offset;
};

#endif