#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hitable.hpp"

class sphere : public hitable
{
public:
    vec3 center;
    float radius;
    material *mat_ptr;

    sphere() = default;
    sphere(vec3 cen, float r, material *ptr) : center(cen), radius(r), mat_ptr(ptr) {}
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
};

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const
{
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction()); // eliminated 2 for discriminant
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;

    if (discriminant > 0)
    {
        float tmp = (-b - sqrt(discriminant)) / a;
        if (tmp < t_max && tmp > t_min)
        {
            rec.t = tmp;
            rec.p = r.point_at_parameter(tmp);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = this->mat_ptr;
            return true;
        }
        tmp = (-b + sqrt(discriminant)) / a;
        if (tmp < t_max && tmp > t_min)
        {
            rec.t = tmp;
            rec.p = r.point_at_parameter(tmp);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = this->mat_ptr;
            return true;
        }
    }
    return false;
}

#endif