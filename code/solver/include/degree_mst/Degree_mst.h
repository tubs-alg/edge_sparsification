#ifndef SPARSIFICATION_DEGREE_MST_H
#define SPARSIFICATION_DEGREE_MST_H

#include <definitions/global_definitions.h>
#include "gurobi_c++.h"
#include "solver/Instance.h"
#include "degree_mst/Degree_mst_solution.h"
#include "algutils/upper_triagular_indexing.hpp"
#include <boost/graph/connected_components.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <CGAL/Delaunay_triangulation_2.h>

#include "utils/gnuplot_output_2.h"

using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Kernel>;

namespace dmst {

    typedef typename std::pair<std::size_t, std::size_t> edge;

    enum Solver_type {
        DEGREE_MST = 0
    };

    std::string solver_type_str(Solver_type type);

    class LazyDegreeMST : public GRBCallback {
    public:
        std::map<std::size_t, GRBVar> xvars;
        Solver::Instance &instance;
        std::size_t n;
        std::size_t cbCnt;
        std::size_t secCnt;

        LazyDegreeMST(std::map<std::size_t, GRBVar> &xv, Solver::Instance &_instance);

    protected:
        void callback() override;

        bool find_violated_sec(const Graph &g);

        void add_subtour_constraint(const std::vector<std::size_t> &indices);

        void plot();
    };

    class Degree_mst {

    private:

        Solver::Instance &instance;
        std::size_t n;
        std::map<std::size_t, GRBVar> xvars;
        std::shared_ptr<Degree_mst_solution> solution;
        Solver_type solver_choice;
        int time_limit;
        int degree;
        bool output;

    public:

        double objVal;

        Degree_mst(Solver::Instance &_instance, Solver_type solver_type, int _degree, bool warm_start = false, int time_limit = 0, bool output = true);

        std::shared_ptr<Degree_mst_solution> getSolution();

        void solveLazy(GRBModel &model);

        void defineModel(GRBModel &model);

        json jump_start_delaunay();

        json jump_start_mst();

    };

}

#endif //SPARSIFICATION_DEGREE_MST_H
