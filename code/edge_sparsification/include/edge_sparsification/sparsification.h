#ifndef SPARSIFICATION_SPARSIFICATION_H
#define SPARSIFICATION_SPARSIFICATION_H

#include <vector>

#include "EdgeSparsification.h"

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Construct_theta_graph_2.h>
#include <CGAL/Construct_yao_graph_2.h>

#include "mwt_library/mwt_library.h"
#include "greedy_triangulation/GreedyTriangulation.h"
#include "wspd/WSPD.h"
#include "greedy_spanner/GreedySpanner.h"
#include "onion_hull/OnionHull.h"
#include "gabriel/GabrielGraph.h"
#include "rng/RelativeNeighborhoodGraph.h"

#include "definitions/global_definitions.h"

using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Kernel>;
using GreedyTriangulation = Sparsification::GreedyTriangulation;
using Traits  = MWT::MWTSolver::Traits;
using MinimumWeightTriangulation = MWT::MWTSolver;

namespace Sparsification {

    void complete_graph(EdgeSparsification & sparsification_instance);

    void delaunay_triangulation(EdgeSparsification & sparsification_instance);

    void greedy_triangulation(EdgeSparsification & sparsification_instance, bool min = true);

    void minimum_weight_triangulation(EdgeSparsification & sparsification_instance);

    void theta(EdgeSparsification & sparsification_instance, int cones_selected, int k);

    void yao(EdgeSparsification & sparsification_instance, int cones_selected, int k);

    void greedy_spanner(EdgeSparsification & sparsification_instance, double t);

    void wspd_spanner(EdgeSparsification & sparsification_instance, double t, int variant, double sep = -1);

    void onion(EdgeSparsification & sparsification_instance, int variant, int k = 0);

    void gabriel_graph(EdgeSparsification & sparsification_instance);

    void relative_neighborhood_graph(EdgeSparsification & sparsification_instance);

}

#endif //SPARSIFICATION_SPARSIFICATION_H
