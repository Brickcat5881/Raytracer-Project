#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

//Parent Guidlines
class material {
    public:
    virtual ~material() = default;

    virtual color emitted() const {
        return color(0, 0, 0); //Default is no emmision
    }

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
            return false;
    }
    
};

//A diffuse material that reflects light at random angles, a matte material like concrete
class lambertian : public material {
    public:
    lambertian(const  color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) 
    const override {
        //Scatters in a slightly random direction but still roughly normal
        auto scatter_direction = rec.normal + random_unit_vector();

        //Catch near zero scatter directions
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

    private:
    color albedo;
};

//Reflective metal with a fuzz (how polished it is)
class metal : public material {
    public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) 
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
    private:
    color albedo;
    double fuzz;
};

//Transparent material 
class dielectric : public material {
    public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        //Basically a bunch of maths which calculates the reflection and refraction
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);

        return true;
    }

    private:
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        //Use Schlik's approximation
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow(1-cosine,5);
    }
};

//Glowing object
class diffuse_light : public material {
    public:
    diffuse_light(color emit) : emit(emit) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        return false; //Absorbs all rays
    }

    color emitted() const override {
        return emit;
    }

    private:
    color emit;
};

#endif