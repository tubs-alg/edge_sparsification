#include "mpp/Min_perimeter_polygon_solution.h"

namespace mpp {

    Min_perimeter_polygon_solution::Min_perimeter_polygon_solution() = default;

    Min_perimeter_polygon_solution::Min_perimeter_polygon_solution(Solver::Instance & _instance, const std::string solver) {
        instance = _instance;
        for (auto & p : instance.points) {
            boost::add_vertex(p, g);
        }
        json_mpp.update(instance.to_json(false, false));
        json_mpp["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        json_mpp["host"] = boost::asio::ip::host_name();
        solver_type = solver;
        json_mpp["id"] = instance.instance_name + "_" + solver_type;
        json_mpp["solver"] = solver_type;
    }

    void Min_perimeter_polygon_solution::to_json(std::string fn) {
        boost::filesystem::ofstream ostream{fn};
        ostream << json_mpp.dump(4);
        ostream.close();
    }

    void Min_perimeter_polygon_solution::put_model_solution(json grbJson) {
        json_mpp.update(grbJson["SolutionInfo"]);
    }

    void Min_perimeter_polygon_solution::put_edges(const std::vector<Graph::edge_descriptor> &edges) {
        solution_edges = edges;
        json json_edges;
        for (auto e : solution_edges) {
            auto i = boost::source(e, instance.g);
            auto j = boost::target(e, instance.g);
            json_edges.emplace_back(json{i, j});
            boost::add_edge(i, j, g);
        }
        json_mpp["solution_edges"] = json_edges;
    }

    void Min_perimeter_polygon_solution::put_json_int(std::string key, int value) {
        json_mpp[key] = value;
    }

    void Min_perimeter_polygon_solution::put_json_double(std::string key, double value) {
        json_mpp[key] = value;
    }

    void Min_perimeter_polygon_solution::plot(std::string fn) {
        CGAL::gnuplot_output_2(g, fn, &solution_edges, true);
    }

}
