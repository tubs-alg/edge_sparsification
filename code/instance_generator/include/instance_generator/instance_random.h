#ifndef SPARSIFICATION_INSTANCE_RANDOM_H
#define SPARSIFICATION_INSTANCE_RANDOM_H

#include <vector>
#include <random>
#include "definitions/global_definitions.h"

namespace instance_generator {

    std::vector<Point_2> uniform_int_random(std::size_t seed, std::size_t size, int a, int b);

    std::vector<Point_2> normal_random(std::size_t seed, std::size_t size, int mean, int stddev);

    std::vector<Point_2> uniform_real_random(std::size_t seed, std::size_t size, int a, int b);

}

#endif //SPARSIFICATION_INSTANCE_RANDOM_H