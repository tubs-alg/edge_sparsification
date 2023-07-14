#ifndef SPARSIFICATION_MIN_PERIMETER_POLYGON_SOLUTION_H
#define SPARSIFICATION_MIN_PERIMETER_POLYGON_SOLUTION_H

#include "solver/Solution.h"
#include "solver/Instance.h"
#include "definitions/global_definitions.h"
#include "utils/json_utils.hpp"
#include "utils/gnuplot_output_2.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace mpp {

class Min_perimeter_polygon_solution : public Solver::Solution {

    public:

        json json_mpp;
        Graph g;
        std::string solver_type;
        Solver::Instance instance;
        std::vector<Graph::edge_descriptor> solution_edges;

    Min_perimeter_polygon_solution();

    Min_perimeter_polygon_solution(Solver::Instance & _instance, const std::string solver);

        void to_json(std::string fn) override;

        void put_json_int(std::string key, int value);

        void put_json_double(std::string key, double value);

        void plot(std::string fn) override;

        void put_model_solution(json grbJson);

        void put_edges(const std::vector<Graph::edge_descriptor> & edges);

};

}

#endif //SPARSIFICATION_MIN_PERIMETER_POLYGON_SOLUTION_H
