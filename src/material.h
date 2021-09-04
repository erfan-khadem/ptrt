#pragma once

#include "common.h"
#include "hittable.h"

class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
        virtual ~material() = default;
    public:
        bool glows = false;
};

class lambertian : public material {
    public:
        lambertian(const color &a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override;

    public:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override;

    public:
        color albedo;
        double fuzz;
};

class dielectric : public material {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction), albedo(1.0, 1.0, 1.0) {}
        dielectric(const color& a, double index_of_refraction) : ir(index_of_refraction), albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override;

    public:
        double ir; // Index of Refraction
        color albedo;
    private:
        static double reflectance(double cosine, double ref_idx);
};

class glowing : public material {
    public:
        glowing(const color& g) : glow_color(g) {glows = true;}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override;

    public:
        color glow_color;
};