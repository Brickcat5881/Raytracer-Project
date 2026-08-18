#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"

class triangle : public hittable {
    public:
    triangle(const point3& a, const point3& b, const point3& c, shared_ptr<material> mat) : a(a), b(b), c(c), mat(mat) {

    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        //Using the Möller-Trumbore intersection algorithm to check if the ray intersects the triangle
        auto edge1 = b-a;
        auto edge2 = c-a;

        point3 normal = unit_vector(cross(edge1, edge2));
        auto x = cross(r.direction(), edge2);
        auto det = dot(x, edge1);
        

        if (std::fabs(det) < 1e-8) {
            return false;
        }

        auto invDet = 1/det;    
        vec3 s1 = r.origin() - a;
        
        auto u = dot(s1,x) * invDet;
        if (u < 0.0 || u > 1.0) {
            return false;
        }

        vec3 s2 = cross(s1,edge1);
        auto v = dot(r.direction(),s2) * invDet;
        if (v < 0.0 || (u+v) > 1.0) {
            return false;
        }

        auto t = dot(edge2,s2) * invDet;
        if (ray_t.surrounds(t)) {
            rec.t = t;
            rec.p = r.at(rec.t);
            rec.set_face_normal(r, normal);
            rec.mat = mat;
            return true;
        }

        return false;
        
    }

    aabb bounding_box() const override {
        auto minX = std::fmin(a.x(), std::fmin(b.x(), c.x()));
        auto maxX = std::fmax(a.x(), std::fmax(b.x(), c.x()));

        auto minY = std::fmin(a.y(), std::fmin(b.y(), c.y()));
        auto maxY = std::fmax(a.y(), std::fmax(b.y(), c.y()));

        auto minZ = std::fmin(a.z(), std::fmin(b.z(), c.z()));
        auto maxZ = std::fmax(a.z(), std::fmax(b.z(), c.z()));

        aabb triangleAABB = aabb(point3(minX,minY,minZ), point3(maxX,maxY,maxZ));
        return triangleAABB;
    }

    private:
    point3 a;
    point3 b;
    point3 c;
    shared_ptr<material> mat;


};

#endif