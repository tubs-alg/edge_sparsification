#include "solver/Instance.h"

namespace Solver {

    Instance::Instance() = default;

    Instance::Instance(Sparsification::EdgeSparsification edge_sparsification) {
        g = edge_sparsification.g;
        points = edge_sparsification.points;
        instance_name = edge_sparsification.getId();
        pointset_name = edge_sparsification.getInstanceName();
    }

    Instance::Instance(const boost::filesystem::path& in, bool scale) {
        g = Graph{};
        if (boost::filesystem::extension(in) == ".json") {
            json json_instance_file;
            boost::filesystem::ifstream ifs{in};
            ifs >> json_instance_file;
            ifs.close();
            double xshift = 0;
            double yshift = 0;
            double max_side_length = 500;
            double max_range = 500;
            if (scale) {
                double xmin = std::numeric_limits<double>::infinity();
                double xmax = -std::numeric_limits<double>::infinity();
                double ymin = std::numeric_limits<double>::infinity();
                double ymax = -std::numeric_limits<double>::infinity();
                for (auto p : json_instance_file["points"]) {
                    double px = p["x"];
                    double py = p["y"];
                    xmin = std::min(xmin, px);
                    xmax = std::max(xmax, px);
                    ymin = std::min(ymin, py);
                    ymax = std::max(ymax, py);
                }
                max_range = std::max(std::abs(xmax-xmin), std::abs(ymax-ymin));
            }
            for (auto p : json_instance_file["points"]) {
                double px = p["x"];
                double py = p["y"];
                if (scale) {
                    px = px * max_side_length / max_range;
                    py = py * max_side_length / max_range;
                }
                Point_2 point = Point_2{px, py};
                points.emplace_back(point);
                boost::add_vertex(point, g);
            }
            if (json_instance_file["sparsification_technique"] == "complete") {
                for (std::size_t i = 0; i < points.size(); ++i) {
                    for (std::size_t j = i+1; j < points.size(); ++j) {
                        boost::add_edge(i, j, g);
                    }
                }
            } else {
                for (auto e : json_instance_file["edges"]) {
                    boost::add_edge(e[0], e[1], g);
                }
            }
            instance_name = json_instance_file["id"];
            pointset_name = json_instance_file["instance"];

        } else {
            Sparsification::EdgeSparsification edge_sparsification{in};
            Sparsification::complete_graph(edge_sparsification);
            g = edge_sparsification.g;
            points = edge_sparsification.points;
            instance_name = edge_sparsification.getId();
            pointset_name = edge_sparsification.getInstanceName();
        }
    }

    json Instance::to_json(bool points_out, bool edges_out) {
        json json_s;

        if (points_out) {
            json_s["points"] = points_to_json(points);
        }

        if (edges_out) {
            json json_edges;
            for (auto e : boost::make_iterator_range(boost::edges(g))) {
                auto s = boost::source(e,g);
                auto t = boost::target(e,g);
                json_edges.emplace_back(json{s, t});
            }
            json_s["edges"] = json_edges;
        }

        json_s["instance"] = instance_name;
        json_s["pointset"] = pointset_name;

        return json_s;
    }

}
