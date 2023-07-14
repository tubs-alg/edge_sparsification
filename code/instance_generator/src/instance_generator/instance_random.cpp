#include "instance_generator/instance_random.h"

namespace instance_generator {

std::vector<Point_2> uniform_int_random(std::size_t seed, std::size_t size, int a, int b) {
    std::vector<Point_2> points;

    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(a,b);

    auto random_point = [&]() { return Point_2{dist(gen), dist(gen)}; };
    for (int i = 0; i < size; ++i) {
        points.emplace_back(random_point());
    }

    return points;
}


std::vector<Point_2> normal_random(std::size_t seed, std::size_t size, int mean, int stddev) {
    std::vector<Point_2> points;

    std::mt19937 gen(seed);
    std::normal_distribution<double> dist(mean, stddev);

    auto random_point = [&]() { return Point_2{dist(gen), dist(gen)}; };
    for (int i = 0; i < size; ++i) {
        points.emplace_back(random_point());
    }

    return points;
}

std::vector<Point_2> uniform_real_random(std::size_t seed, std::size_t size, int a, int b) {
    std::vector<Point_2> points;

    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(a, b);

    auto random_point = [&]() { return Point_2{dist(gen), dist(gen)}; };
    for (int i = 0; i < size; ++i) {
        points.emplace_back(random_point());
    }

    return points;
}

}