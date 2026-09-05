#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"

class material;

//Essentially a list of what has been it and the information about each hit and the ray
class hit_record {
    public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        //Sets the hit record normal vector
        //outward_normal assumed to have unit length!!

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

//Base for hittable but is empty as it must be overriden by the child
class hittable {
    public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
    virtual aabb bounding_box() const = 0;
};
#endif