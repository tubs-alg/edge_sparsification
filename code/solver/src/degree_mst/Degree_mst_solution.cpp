#include "degree_mst/Degree_mst_solution.h"

namespace dmst {

    Degree_mst_solution::Degree_mst_solution() = default;

    Degree_mst_solution::Degree_mst_solution(Solver::Instance &_instance, const std::string& solver) {
        instance = _instance;
        for (auto &p : instance.points) {
            boost::add_vertex(p, g);
        }
        json_dmst.update(instance.to_json(false, false));
        json_dmst["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        json_dmst["host"] = boost::asio::ip::host_name();
        solver_type = solver;
        json_dmst["id"] = instance.instance_name + "_" + solver_type;
        json_dmst["solver"] = solver_type;
    }

    void Degree_mst_solution::to_json(std::string fn) {
        boost::filesystem::ofstream ostream{fn};
        ostream << json_dmst.dump(4);
        ostream.close();
    }

    void Degree_mst_solution::put_model_solution(json grbJson) {
        json_dmst.update(grbJson["SolutionInfo"]);
    }

    void Degree_mst_solution::put_edges(const std::vector<Graph::edge_descriptor> &edges) {
        solution_edges = edges;
        json json_edges;
        for (auto e : solution_edges) {
            auto i = boost::source(e, instance.g);
            auto j = boost::target(e, instance.g);
            json_edges.emplace_back(json{i, j});
            boost::add_edge(i, j, g);
        }
        json_dmst["solution_edges"] = json_edges;
    }

    void Degree_mst_solution::put_json_size_t(const std::string& key, std::size_t value) {
        json_dmst[key] = value;
    }

    void Degree_mst_solution::put_json_int(const std::string& key, int value) {
        json_dmst[key] = value;
    }

    void Degree_mst_solution::put_json_double(const std::string& key, double value) {
        json_dmst[key] = value;
    }

    void Degree_mst_solution::plot(std::string fn) {
        CGAL::gnuplot_output_2(g, fn, &solution_edges, true);
    }

}
