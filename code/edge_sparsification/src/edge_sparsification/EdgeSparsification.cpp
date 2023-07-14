#include "edge_sparsification/EdgeSparsification.h"

namespace Sparsification {

    EdgeSparsification::EdgeSparsification() = default;

    EdgeSparsification::EdgeSparsification(std::vector<Point_2> & _points) {
        for (auto & p : _points) {
            insertPoint(p);
        }
    }

    EdgeSparsification::EdgeSparsification(const boost::filesystem::path& in) {
        /*
         * @in: Path to point instance file
         */
        if (boost::filesystem::extension(in) == ".json") {
            json json_instance_file;
            boost::filesystem::ifstream ifs{in};
            ifs >> json_instance_file;
            ifs.close();
            for (auto p : json_instance_file["points"]) {
                insertPoint(Point_2{p["x"], p["y"]});
            }
            instance_name = json_instance_file["id"].get<std::string>();
        } else {
            for (auto p : read_file<std::vector<Point_2>>(in)) {
                insertPoint(p);
            }
            instance_name = boost::filesystem::path(in).stem().string();
        }
    }

    EdgeSparsification::EdgeSparsification(std::size_t seed, std::size_t size, const std::string& type) {
        bool has_duplicates = true;
        std::vector<Point_2> _points{size};
        while (has_duplicates) {
            std::cout << "Seed: " << seed << std::endl;
            if (type == "normal") {
                _points = instance_generator::normal_random(seed, size, 0, 1);
            } else if (type == "uniform_real") {
                _points = instance_generator::uniform_real_random(seed, size, 0, 1);
            } else if (type == "uniform_int") {
                _points = instance_generator::uniform_int_random(seed, size, 0, 500);
            }
            std::sort(_points.begin(), _points.end());
            has_duplicates = std::adjacent_find(_points.begin(), _points.end()) != _points.end();
            if(has_duplicates) {
                std::cout << "Duplicate points found!" << std::endl;
                _points.clear();
            }
        }
        for (auto p : _points) {
            insertPoint(p);
        }
        instance_name = "random_" + type + "_"
                   + boost::lexical_cast<std::string>(size)
                   + "_" + boost::lexical_cast<std::string>(seed);
    }

    bool EdgeSparsification::insertPoint(Point_2 p) {
        if (point_map.count(p)) {
            std::cout << "Point already existing: " << p << std::endl;
            return false;
        }
        points.emplace_back(p);
        out_degrees.emplace_back(0);
        boost::add_vertex(p, g);
        point_map[p] = num_vertices++;
        return true;
    }

    bool EdgeSparsification::insertEdge(std::size_t s, std::size_t t) {
        if (s >= points.size() || t >= points.size()) {
            std::cout << "At least one point missing: " << s << " - " << t << std::endl;
            return false;
        }
        if (boost::edge(s, t, g).second) {
//            std::cout << "Edge already existing: " << s << " - " << t << std::endl;
            return false;
        }
        double distance = CGAL::sqrt(CGAL::squared_distance(g[s], g[t]));
        boost::add_edge(s, t, distance, g);
        graph_weight += distance;
        out_degrees[s]++;
        out_degrees[t]++;
        num_edges++;
        stretch_up_to_date = false;
        return true;
    }

    bool EdgeSparsification::insertEdge(Point_2 p_s, Point_2 p_t) {
        if (!(point_map.count(p_s) && point_map.count(p_t))) {
            std::cout << "At least one point missing: " << p_s << " - " << p_t << std::endl;
            return false;
        }
        auto s = point_map[p_s];
        auto t = point_map[p_t];
        return insertEdge(s, t);
    }

    void EdgeSparsification::compute_stretch(bool stretch_analysis) {
        if (!stretch_up_to_date) {

            if (sparsification_type == "complete") {
                max_stretch = 1;
            } else {
                std::vector<std::vector<double>> distances(boost::num_vertices(g));
                for (auto &vec : distances) {
                    vec.resize(boost::num_vertices(g));
                }
                boost::johnson_all_pairs_shortest_paths(g, distances);

                for (std::size_t i = 0; i < boost::num_vertices(g); ++i) {
                    for (std::size_t j = i+1; j < boost::num_vertices(g); ++j) {
                        auto euclidean_distance = CGAL::sqrt(CGAL::squared_distance(points[i], points[j]));
                        if (distances[i][j] == std::numeric_limits<double>::max()) {
                            // graph not connected
                            max_stretch = std::numeric_limits<double>::infinity();
                        } else {
                            auto ratio = distances[i][j] / euclidean_distance;
                            all_pairs_stretch.emplace_back(ratio);
                            if (ratio > max_stretch) {
                                max_stretch = ratio;
                                max_stretch_pair = std::make_pair(i, j);
                            }
                        }
                    }
                }
            }
            stretch_up_to_date = true;
        }
    }

    void EdgeSparsification::to_json(const boost::filesystem::path& out, bool include_points, bool include_edges) {
        if (stretch_up_to_date) {
            json_s["max_stretch"] = max_stretch;
            json_s["max_stretch_pair"] = max_stretch_pair;
            if (!all_pairs_stretch.empty()) {
                auto const Q1 = all_pairs_stretch.size() / 4;
                auto const Q2 = all_pairs_stretch.size() / 2;
                auto const Q3 = Q1 + Q2;
                auto const P10 = all_pairs_stretch.size() / 10;
                auto const P90 = all_pairs_stretch.size() - P10;

                std::nth_element(all_pairs_stretch.begin(), all_pairs_stretch.begin() + P10, all_pairs_stretch.end());
                std::nth_element(all_pairs_stretch.begin() + P10 + 1, all_pairs_stretch.begin() + Q1, all_pairs_stretch.end());
                std::nth_element(all_pairs_stretch.begin() + Q1 + 1, all_pairs_stretch.begin() + Q2, all_pairs_stretch.end());
                std::nth_element(all_pairs_stretch.begin() + Q2 + 1, all_pairs_stretch.begin() + Q3, all_pairs_stretch.end());
                std::nth_element(all_pairs_stretch.begin() + Q3 + 1, all_pairs_stretch.begin() + P90, all_pairs_stretch.end());
                json_s["stretch_Q25"] = all_pairs_stretch.at(Q1);
                json_s["stretch_median"] = all_pairs_stretch.at(Q2);
                json_s["stretch_Q75"] = all_pairs_stretch.at(Q3);
                json_s["stretch_P10"] = all_pairs_stretch.at(P10);
                json_s["stretch_P90"] = all_pairs_stretch.at(P90);
                double sum = std::accumulate(std::begin(all_pairs_stretch), std::end(all_pairs_stretch), 0.0);
                double m =  sum / all_pairs_stretch.size();

                double accum = 0.0;
                std::for_each (std::begin(all_pairs_stretch), std::end(all_pairs_stretch), [&](const double d) {
                    accum += (d - m) * (d - m);
                });

                double stdev = sqrt(accum / all_pairs_stretch.size());
                json_s["stretch_mean"] = m;
                json_s["stretch_stddev"] = stdev;
            }
        }
        if (include_points) {
            json_s["points"] = points_to_json(points);
        }
        if (include_edges && sparsification_type != "complete") {
            json json_edges;
            auto edges = boost::edges(g);
            for (auto e = edges.first; e != edges.second; ++e) {
                auto s = boost::source(*e,g);
                auto t = boost::target(*e,g);
                json_edges.emplace_back(json{s, t});
            }
            json_s["edges"] = json_edges;
        }
        json_s["sparsification_technique"] = sparsification_type;
        json_s["graphWeight"] = graph_weight;
        json_s["num_vertices"] = num_vertices;
        json_s["num_edges"] = num_edges;
        json_s["instance"] = instance_name;
        json_s["out_degrees"] = out_degrees;
        json_s["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        json_s["host"] = boost::asio::ip::host_name();
        json_s["id"] = getId();

        std::string fn = getId() + ".json";
        boost::filesystem::ofstream ostream{boost::filesystem::complete(fn , out)};
        ostream << json_s.dump(4);
        ostream.close();
        std::cout << boost::filesystem::complete(fn, out).string() << std::endl;
    }

    void EdgeSparsification::print( ) {
        std::cout << "Sparsification technique: " << sparsification_type << std::endl;
        std::cout << "num vertices:\t" << boost::num_vertices(g) << std::endl;
        std::cout << "graph weight:\t" << graph_weight << std::endl;
        std::cout << "num edges:\t\t" << boost::num_edges(g) << std::endl;
        if (stretch_up_to_date) {
            std::cout << "stretch factor:\t\t" << max_stretch << std::endl;
            std::cout << "max stretch pair:\t" << max_stretch_pair.first << "-" << max_stretch_pair.second <<  std::endl;
        }
    }

    void EdgeSparsification::plot(const boost::filesystem::path& out) const {
        CGAL::gnuplot_output_2(g, boost::filesystem::complete(getId(), out.string()).string(), nullptr , true);
    }

    std::size_t EdgeSparsification::num_points() const {
        return boost::num_vertices(g);
    }

    std::string EdgeSparsification::getId() const {
        return instance_name + "_" + sparsification_type;
    }

    void EdgeSparsification::mergeEdges(EdgeSparsification & B) {
        if (instance_name != B.instance_name) {
            std::cout << "DIFFERENT POINTSETS!" << std::endl;
            return;
        }
        sparsification_type += "-" + B.sparsification_type;
        auto edges_tbi = boost::edges(B.g);
        for (auto e = edges_tbi.first; e != edges_tbi.second; ++e) {
            auto s = boost::source(*e,g);
            auto t = boost::target(*e,g);
            if (insertEdge(s, t)) {
                std::cout << "Edge inserted: " << B.sparsification_type << " " << s << " - " << t << std::endl;
            }
        }
    }

    void EdgeSparsification::setInstanceName(const std::string &instanceName) {
        instance_name = instanceName;
    }

    void EdgeSparsification::setSparsificationType(const std::string &sparsificationType) {
        sparsification_type = sparsificationType;
    }

    const std::string &EdgeSparsification::getInstanceName() const {
        return instance_name;
    }

    double EdgeSparsification::getMaxStretch() {
        compute_stretch(false);
        return max_stretch;
   }

   double EdgeSparsification::getGraphWeight() {
        return graph_weight;
    }

    EdgeSparsification from_json(const boost::filesystem::path &in) {
        /*
         * @in: Path to edge sparsification json file
         */
        EdgeSparsification edgeSparsification{};
        if (boost::filesystem::extension(in) != ".json") {
            std::cout << "No JSON EdgeSparsification file!" << std::endl;
            return edgeSparsification;
        }
        json json_edge_sparsification;
        boost::filesystem::ifstream ifs{in};
        ifs >> json_edge_sparsification;
        ifs.close();
        edgeSparsification.setSparsificationType(json_edge_sparsification["sparsification_technique"]);
        edgeSparsification.setInstanceName(json_edge_sparsification["instance"]);
        for (auto p : json_edge_sparsification["points"]) {
            edgeSparsification.insertPoint(Point_2{p["x"], p["y"]});
        }
        if (json_edge_sparsification["sparsification_technique"] == "complete") {
            for (std::size_t i = 0; i < edgeSparsification.num_points(); ++i) {
                for (std::size_t j = i+1; j < edgeSparsification.num_points(); ++j) {
                    edgeSparsification.insertEdge(i, j);
                }
            }
        } else {
            for (auto e : json_edge_sparsification["edges"]) {
                edgeSparsification.insertEdge(e[0], e[1]);
            }
        }
        return edgeSparsification;
    }

}
