#include "mwt_library/mwt_library.h"

namespace MWT {

    MWTSolver::return_type_epeck MWTSolver::solve(std::vector<Epeck::Point_2> &points) {
        std::vector<Point_2> new_points;

        for (auto &point: points) {
            new_points.emplace_back(Point_2(CGAL::to_double(point.x()), CGAL::to_double(point.y())));
        }

        auto result = this->solve(new_points);

        std::vector<Epeck_Edge> new_edges;

        for (auto &edge: result.edges) {
            new_edges.emplace_back(
                    Epeck_Edge(
                            Epeck::Point_2(CGAL::to_double(edge.first.x()), CGAL::to_double(edge.first.y())),
                            Epeck::Point_2(CGAL::to_double(edge.second.x()), CGAL::to_double(edge.second.y()))));
        }

        return return_type_epeck{result.solved, new_edges, result.value, result.time, result.stats};
    }

    MWTSolver::return_type_epick MWTSolver::solve(std::vector<Point_2> &points) {
        std::vector<Edge> edges;
        double value = 0;
        Mwt_stats stats;
        auto time = measure_time<std::chrono::milliseconds>([&] {
            CGAL::Minimum_weight_triangulation_2<Traits, CGAL::Sequential_tag> mwt{points.begin(), points.end(), false,
                                                                                   true, true};
            //std::cout << mwt.statistics() << std::endl;
            stats = mwt.statistics();
            mwt.output_mwt(std::back_inserter(edges));

            for (auto &e : edges) {
                value += std::sqrt(CGAL::squared_distance(e.first, e.second));
            }
        }).count();

        return return_type_epick{true, edges, value, time, stats};
    }

}