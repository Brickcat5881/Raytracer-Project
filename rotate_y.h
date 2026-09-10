#ifndef ROTATE_Y_H
#define ROTATE_Y_H

#include "hittable.h"
#include "aabb.h"

//Used to move hittable objects in the scene
//Moves the rays instead of the object which is easier
class rotate_y: public hittable {
    public:
    rotate_y(shared_ptr<hittable> object, const double theta) : object(object) {
        auto radians = degrees_to_radians(theta);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point3 min(infinity, infinity, infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                    auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                    auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    } 

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        //Transform ray from world space to object space
        auto origin = point3(
            (cos_theta * r.origin().x()) - (sin_theta * r.origin().z()), r.origin().y(),
            (sin_theta * r.origin().x() + (cos_theta * r.origin().z())));
        
        auto direction = vec3(
            (cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
            r.direction().y(),
            (sin_theta * r.direction().x()) + (cos_theta * r.direction().z()));
        
        ray rotated_r(origin, direction);
        
        //Do actual hit check with new rotated ray
        if (!object->hit(rotated_r, ray_t, rec)) {return false;}
        
        rec.p = point3(
            (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
            rec.p.y(),
            (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z()));
        
        rec.normal = vec3(
            (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
            rec.normal.y(),
            (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z()));
        
        return true;

    }

    aabb bounding_box() const override {
        return bbox;
    }
    private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};

#endif