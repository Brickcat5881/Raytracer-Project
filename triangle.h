#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"


class triangle : public hittable {
    public:
    triangle(const point3& a, const point3& b, const point3& c, shared_ptr<material> mat) : a(a), b(b), c(c) {

    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        
    }


}