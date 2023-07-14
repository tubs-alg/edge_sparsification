#include "angle_tsp/Angle_tsp_solution.h"

angle_tsp::Angle_tsp_solution::Angle_tsp_solution() = default;

angle_tsp::Angle_tsp_solution::Angle_tsp_solution(Solver::Instance & _instance, const std::string solver) {
    instance = _instance;
    for (auto & p : instance.points) {
        boost::add_vertex(p, g);
    }
    json_ang.update(instance.to_json(false, false));
    json_ang["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    json_ang["host"] = boost::asio::ip::host_name();
    solver_type = solver;
    json_ang["id"] = instance.instance_name + "_" + solver_type;
    json_ang["solver"] = solver_type;
}

void angle_tsp::Angle_tsp_solution::to_json(std::string fn) {
    json_ang["tour"] = get_tour();
    boost::filesystem::ofstream ostream{fn};
    ostream << json_ang.dump(4);
    ostream.close();
}

void angle_tsp::Angle_tsp_solution::put_model_solution(json grbJson) {
    json_ang.update(grbJson["SolutionInfo"]);
}

void angle_tsp::Angle_tsp_solution::plot(std::string fn) {
    CGAL::gnuplot_output_2(g, fn, &tour_edges, true);
}

std::vector<int> angle_tsp::Angle_tsp_solution::get_tour() {
    std::vector<int> tour;
    if (tour_edges.empty()) return tour;
    auto s = boost::source(tour_edges[0],instance.g);
    auto t = boost::target(tour_edges[0],instance.g);
    tour.emplace_back(s);
    auto e = std::vector<bool>(tour_edges.size(), false);
    e[0] = true;
    do {
        for (int i = 0; i < tour_edges.size(); ++i) {
            if (e[i]) continue;
            auto v1 = boost::source(tour_edges[i], instance.g);
            auto v2 = boost::target(tour_edges[i], instance.g);
            if (v1 == t) {
                tour.emplace_back(t);
                s = t;
                t = v2;
                e[i] = true;
            } else if (v2 == t) {
                tour.emplace_back(t);
                s = t;
                t = v1;
                e[i] = true;
            }
        }
    } while (t != tour[0]);

    return tour;
}

void angle_tsp::Angle_tsp_solution::put_tour_edges(const std::vector<Graph::edge_descriptor> &edges) {
    tour_edges = edges;
    json json_edges;
    for (auto e : tour_edges) {
        auto i = boost::source(e, instance.g);
        auto j = boost::target(e, instance.g);
        json_edges.emplace_back(json{i, j});
        boost::add_edge(i, j, g);
    }
    json_ang["solution_edges"] = json_edges;
}

void angle_tsp::Angle_tsp_solution::put_json_int(std::string key, int value) {
    json_ang[key] = value;
}

void angle_tsp::Angle_tsp_solution::put_json_double(std::string key, double value) {
    json_ang[key] = value;
}
