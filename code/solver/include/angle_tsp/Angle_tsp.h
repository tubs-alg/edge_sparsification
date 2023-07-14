#ifndef SPARSIFICATION_ANGLE_TSP_H
#define SPARSIFICATION_ANGLE_TSP_H

#include <chrono>

#include <boost/asio/ip/host_name.hpp>
#include <definitions/global_definitions.h>

#include "gurobi_c++.h"

#include "solver/Instance.h"
#include "angle_tsp/Angle_tsp_solution.h"
#include "nlohmann/json.hpp"

#include <boost/graph/one_bit_color_map.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>

#define printArr(arr, len) for(int i=0;i<len;++i) {std::cout<<arr[i]<<',';} std::cout<<std::endl;

using json = nlohmann::json;

namespace angle_tsp {

    typedef typename std::pair<int, int> edge;
    typedef typename std::tuple<int, int, int> triple;

    const double MULTIPLIER = std::pow(10.0, 12);

    enum Solver_type {
        ANGLE = 0,
        ANGLE_LB = 1,
        ANGLE_DISTANCE = 2,
        ANGLE_DISTANCE_LB = 3
    };

    std::string solver_type_str(Solver_type type);

    double turning_angle_ab(Vector_2 &a, Vector_2 &b);

    double angle_cost(Vector_2 &a, Vector_2 &b);

    double angle_distance_cost(Vector_2 &a, Vector_2 &b, std::size_t rho, std::size_t lambda1 = 100,
                               std::size_t lambda2 = 100);

    class LazySubtour : public GRBCallback {
    public:
        std::map<edge, GRBVar> xvars;
        Graph g;
        std::size_t n;
        bool all_subtours;
        int cbCnt;
        int lazyConstrCnt;

        LazySubtour(std::map<edge, GRBVar> &xv, Graph &graph, std::size_t num_nodes, bool allSubtours);

    protected:
        void callback();

        void add_constraint_type_1(const std::vector<std::size_t> &tour, int len);

        void add_constraint_type_2(const std::vector<std::size_t> &tour, int len);

        int find_subtour(std::vector<bool> &visited, std::vector<std::size_t> &tour, std::size_t start);

        void plot_integral_cb_solution();
    };

    class Angle_tsp {
    private:

        Solver::Instance &instance;
        std::map<edge, GRBVar> xvars;
        std::map<triple, GRBVar> yvars;
        std::shared_ptr<Angle_tsp_solution> solution;
        Solver_type solver_choice;

    public:

        explicit Angle_tsp(Solver::Instance &_instance, Solver_type solver_type, bool lazy = true,
                           bool all_subtours = false, int time_limit = 0);

        std::shared_ptr<Angle_tsp_solution> getSolution();

        void solveLazy(GRBModel &model, bool all_subtours);

        void solveIncrementally(GRBModel &model, bool all_subtours);

        void solveLb(GRBModel &model);

        void defineModel(GRBModel &model);
    };

}

#endif //SPARSIFICATION_ANGLE_TSP_H
