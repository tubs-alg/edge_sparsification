#ifndef SPARSIFICATION_JSON_UTILS_HPP
#define SPARSIFICATION_JSON_UTILS_HPP

#include "nlohmann/json.hpp"

using json = nlohmann::json;

template<class Container>
json points_to_json(Container & points) {

    json json_points = {};

    int j = 0;
    for (auto & p : points) {
        json_points[j++] = { {"x", p.x()}, {"y", p.y()} };
    }

    return json_points;

}

#endif //SPARSIFICATION_JSON_UTILS_HPP
