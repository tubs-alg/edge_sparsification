#ifndef MWT_LIBRARY_HPP
#define MWT_LIBRARY_HPP

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <iterator>

//#define  CGAL_CHECK_EXPENSIVE

#include <map>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/tags.h>

#include "mwt/CGAL/Mwt_traits.h"
#include "mwt/CGAL/Minimum_weight_triangulation.h"

#include "mwt/CGAL/profiling.h"

#include "mwt/CPLEX/basic_mwt_solver.hpp"

namespace MWT
{
    class MWTSolver
    {
    public:
        using Epeck   = CGAL::Exact_predicates_exact_constructions_kernel;
        using Epick   = CGAL::Exact_predicates_inexact_constructions_kernel;
        using Kernel  = Epick;
        using Traits  = typename CGAL::Minimum_weight_triangulation::Mwt_traits_2<Kernel>;
        using Point_2 = typename Kernel::Point_2;
        using FT      = typename Kernel::FT;

        using Edge = std::pair<Point_2, Point_2>;
        using Epeck_Edge = std::pair<Epeck::Point_2, Epeck::Point_2>;

        struct return_type_epeck
        {
            bool solved;
            std::vector<Epeck_Edge> edges;
            double value;
            long long int time; // In milliseconds
            Mwt_stats stats;
        };

        struct return_type_epick
        {
            bool solved;
            std::vector<Edge> edges;
            double value;
            long long int time; // In milliseconds
            Mwt_stats stats;
        };

        return_type_epeck solve(std::vector<Epeck::Point_2> &points);

        return_type_epick solve(std::vector<Point_2> &points);
    };
}

#endif