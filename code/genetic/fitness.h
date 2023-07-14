#ifndef SPARSIFICATION_FITNESS_H
#define SPARSIFICATION_FITNESS_H

#include <vector>

#include "edge_sparsification/EdgeSparsification.h"
#include "edge_sparsification/sparsification.h"
#include "degree_mst/Degree_mst.h"
#include "degree_mst/Degree_mst_solution.h"
#include "definitions/global_definitions.h"

double fitness_degree_mst(std::vector<Point_2> & points) {
    Sparsification::EdgeSparsification sparsification{points};
    Sparsification::complete_graph(sparsification);
    Solver::Instance complete_instance{sparsification};
    dmst::Degree_mst degreeMst{complete_instance, dmst::DEGREE_MST, 3, true, 600, false};

    double objVal_complete = degreeMst.objVal;

    Sparsification::EdgeSparsification dt_sparsification{points};
    Sparsification::delaunay_triangulation(dt_sparsification);
    Solver::Instance dt_instance{dt_sparsification};
    dmst::Degree_mst degreeMstDt{dt_instance, dmst::DEGREE_MST, 3, true, 600, false};

    double objVal_Dt = degreeMstDt.objVal;
    if (objVal_Dt < 1e-4) {
        return 0;
    }

    double fitness = std::abs((objVal_complete - objVal_Dt) / objVal_complete);

    return fitness;
}

double fitness_stretch(std::vector<Point_2> & points) {
    Sparsification::EdgeSparsification sparsification{points};
    Sparsification::minimum_weight_triangulation(sparsification);
    double spanning_ratio = sparsification.getMaxStretch();

    return spanning_ratio;
}

double fitness_delaunay_mwt_gap(std::vector<Point_2> & points) {
    Sparsification::EdgeSparsification sparsification{points};
    Sparsification::minimum_weight_triangulation(sparsification);
    double mwt_weight = sparsification.getGraphWeight();

    Sparsification::EdgeSparsification sparsification_dt{points};
    Sparsification::delaunay_triangulation(sparsification_dt);
    double dt_weight = sparsification_dt.getGraphWeight();

    return dt_weight / mwt_weight;
}

#endif //SPARSIFICATION_FITNESS_H
