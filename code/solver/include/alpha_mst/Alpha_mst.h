#ifndef SPARSIFICATION_ALPHA_MST_H
#define SPARSIFICATION_ALPHA_MST_H

#include <definitions/global_definitions.h>
#include "gurobi_c++.h"
#include "solver/Instance.h"
#include "alpha_mst/Alpha_mst_solution.h"
#include "algutils/upper_triagular_indexing.hpp"
#include <boost/graph/connected_components.hpp>
#include "utils/gnuplot_output_2.h"

namespace amst {

    typedef typename std::pair<std::size_t, std::size_t> edge;

    enum Solver_type {
        ALPHA_MST = 0
    };

    std::string solver_type_str(Solver_type type);

    double directed_angle_k_i_j(Direction_2 &j, Direction_2 &k);

    class LazyAlphaMST : public GRBCallback {
    public:
        std::map<std::size_t, GRBVar> xvars;
        Solver::Instance &instance;
        std::size_t n;
        std::size_t cbCnt;
        std::size_t secCnt;

        LazyAlphaMST(std::map<std::size_t, GRBVar> &xv, Solver::Instance &_instance);

    protected:
        void callback() override;

        bool find_violated_sec(const Graph &g);

        void add_subtour_constraint(const std::vector<std::size_t> &indices);

        void plot();

    };

    class Alpha_mst {

    private:

        Solver::Instance &instance;
        std::size_t n;
        std::map<std::size_t, GRBVar> xvars;
        std::map<edge, GRBVar> yvars;
        std::vector<std::map<std::size_t, double>> angle_0_i_j;
        std::shared_ptr<Alpha_mst_solution> solution;
        Solver_type solver_choice;
        int time_limit;
        double alpha;

    public:

        Alpha_mst(Solver::Instance &_instance, Solver_type solver_type, double _alpha, int time_limit = 0);

        std::shared_ptr<Alpha_mst_solution> getSolution();

        void init_angles();

        void solveLazy(GRBModel &model);

        void defineModel(GRBModel &model);

    };

}

#endif //SPARSIFICATION_ALPHA_MST_H
