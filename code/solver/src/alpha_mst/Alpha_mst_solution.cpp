#include "alpha_mst/Alpha_mst_solution.h"

namespace amst {

    Alpha_mst_solution::Alpha_mst_solution() = default;

    Alpha_mst_solution::Alpha_mst_solution(Solver::Instance &_instance, const std::string& solver) {
        instance = _instance;
        for (auto &p : instance.points) {
            boost::add_vertex(p, g);
        }
        json_amst.update(instance.to_json(false, false));
        json_amst["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        json_amst["host"] = boost::asio::ip::host_name();
        solver_type = solver;
        json_amst["id"] = instance.instance_name + "_" + solver_type;
        json_amst["solver"] = solver_type;
    }

    void Alpha_mst_solution::to_json(std::string fn) {
        boost::filesystem::ofstream ostream{fn};
        ostream << json_amst.dump(4);
        ostream.close();
    }

    void Alpha_mst_solution::put_model_solution(json grbJson) {
        json_amst.update(grbJson["SolutionInfo"]);
    }

    void Alpha_mst_solution::put_edges(const std::vector<Graph::edge_descriptor> &edges) {
        solution_edges = edges;
        json json_edges;
        for (auto e : solution_edges) {
            auto i = boost::source(e, instance.g);
            auto j = boost::target(e, instance.g);
            json_edges.emplace_back(json{i, j});
            boost::add_edge(i, j, g);
        }
        json_amst["solution_edges"] = json_edges;
    }

    void Alpha_mst_solution::put_cones(const std::vector<std::pair<std::size_t, std::size_t>> &edges) {
        json json_edges;
        for (auto e : edges) {
            json_edges.emplace_back(json{e.first, e.second});
        }
        json_amst["solution_cones"] = json_edges;
    }

    void Alpha_mst_solution::put_json_size_t(const std::string& key, std::size_t value) {
        json_amst[key] = value;
    }

    void Alpha_mst_solution::put_json_int(const std::string& key, int value) {
        json_amst[key] = value;
    }

    void Alpha_mst_solution::put_json_double(const std::string& key, double value) {
        json_amst[key] = value;
    }

    void Alpha_mst_solution::plot(std::string fn) {
        CGAL::gnuplot_output_2(g, fn, &solution_edges, true);
    }

}
