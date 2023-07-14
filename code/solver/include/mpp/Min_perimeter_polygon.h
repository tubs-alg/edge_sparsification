#ifndef SPARSIFICATION_MIN_PERIMETER_POLYGON_H
#define SPARSIFICATION_MIN_PERIMETER_POLYGON_H

#include <definitions/global_definitions.h>
#include "gurobi_c++.h"
#include "solver/Instance.h"
#include "mpp/Min_perimeter_polygon_solution.h"
#include "algutils/upper_triagular_indexing.hpp"
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>

#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_id_2.h>

#include "edge_sparsification/EdgeSparsification.h"
#include "edge_sparsification/sparsification.h"

#include "utils/gnuplot_output_2.h"

typedef CGAL::Convex_hull_traits_adapter_2<Kernel , CGAL::Pointer_property_map<Point_2>::type> Convex_hull_traits_2;
typedef CGAL::Arr_segment_traits_2<Epeck> Traits_2;
typedef CGAL::Arrangement_2<Traits_2> Arrangement_2;
typedef Traits_2::X_monotone_curve_2 Arr_Segment_2;
typedef Epeck::Point_2 Point;
typedef Epeck::Segment_2 Segment;

using vb = CGAL::Triangulation_vertex_base_with_id_2<Epeck>;
using fb = CGAL::Constrained_triangulation_face_base_2<Epeck>;
using TDS = CGAL::Triangulation_data_structure_2<vb, fb>;
using ConstrainedDelaunay = CGAL::Constrained_Delaunay_triangulation_2<Epeck, TDS>;
using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Kernel>;

namespace mpp {

    void print_ccb (Arrangement_2::Ccb_halfedge_const_circulator circ, std::map<Point, std::size_t> point_map);

    void print_face (Arrangement_2::Face_const_handle f, std::map<Point, std::size_t> point_map);

    void print_arrangement (const Arrangement_2& arr, const std::map<Point, std::size_t>& point_map);

    typedef typename std::pair<std::size_t, std::size_t> edge;

    enum Solver_type {
        MPP = 0
    };

    std::string solver_type_str(Solver_type type);

    class LazyCycle: public GRBCallback
    {
    public:
        std::map<edge, GRBVar> xvars;
        std::vector<Point> points;
        std::map<Point, std::size_t> point_map;
        Solver::Instance & instance;
        std::size_t n;
        std::size_t cbCnt;
        std::size_t cycleCutCnt;
        std::size_t glueCutCnt;
        std::size_t tailCutCnt;
        std::size_t hihCutCnt;
        std::vector<bool> ch_indices;

        LazyCycle(std::map<edge, GRBVar>& xv, Solver::Instance &_instance);

    protected:
        void callback() override;

        void add_constraint_type_1(const std::vector<std::size_t> & cycle);

        void add_constraint_type_2(const std::vector<std::size_t> & cycle);

        void add_glue_cut(const std::vector<Point> & cut);

        void add_tail_cut(const std::vector<Point> & cut, std::vector<std::size_t> indices);

        void add_hih_cut(const std::vector<Point> & cut, std::vector<std::size_t> indices);

        void plot();

        void plot_with_cut(const std::vector<Point> & cut);
    };

    class Min_perimeter_polygon {

    private:

        Solver::Instance & instance;
        std::map<edge, GRBVar> xvars;
        std::shared_ptr<Min_perimeter_polygon_solution> solution;
        Solver_type solver_choice;
        int time_limit;

    public:

        Min_perimeter_polygon(Solver::Instance & _instance, Solver_type solver_type, std::vector<std::pair<std::size_t, std::size_t>> &ws_edges, bool lazy = true, bool warm_start = false, int time_limit = 0);

        std::shared_ptr<Min_perimeter_polygon_solution> getSolution();

        void solveLazy(GRBModel &model);

        void solveIncrementally(GRBModel &model);

        void defineModel(GRBModel &model);

        json jump_start_delaunay();

        json jump_start_mwt();

    };

}

#endif //SPARSIFICATION_MIN_PERIMETER_POLYGON_H
