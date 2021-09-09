#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list : public hittable {
    public:
        hittable_list() {}
        hittable_list(std::shared_ptr<hittable> object);

        void clear();
        void add(std::shared_ptr<hittable> object);

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        std::vector<std::shared_ptr<hittable>> objects;
};
