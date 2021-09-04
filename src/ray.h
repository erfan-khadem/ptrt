#pragma once

#include "constants.h"
#include "vec3.h"

class ray {
    public:
        ray(){}
        ray(const point3& origin, const vec3& direction)
            : orig(origin), dir(direction)
        {}

        inline point3 origin() const {return orig;}
        inline vec3 direction() const {return dir;}

        inline point3 at(double t) const {
            return orig + t*dir;
        }

    public:
        point3 orig;
        point3 dir;
};

struct ray_return{
    color ray_color;
    int depth;
};