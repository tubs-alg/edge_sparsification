#ifndef SPARSIFICATION_EDGESPARSIFICATION_H
#define SPARSIFICATION_EDGESPARSIFICATION_H

#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/asio/ip/host_name.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#include "instance_generator/instance_random.h"
#include "definitions/global_definitions.h"
#include "utils/fileutils.hpp"
#include "utils/json_utils.hpp"
#include "utils/gnuplot_output_2.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Sparsification {

    class EdgeSparsification {

    private:
        std::string instance_name;
        std::string sparsification_type;
        std::vector<std::size_t> out_degrees;
        double graph_weight{0};
        double max_stretch{0};
        std::size_t num_vertices{0};
        std::size_t num_edges{0};
        std::pair<std::size_t, std::size_t> max_stretch_pair;
        std::vector<double> all_pairs_stretch;
        bool stretch_up_to_date{false};

    public:
        json json_s;
        std::vector<Point_2> points;
        std::map<Point_2, std::size_t> point_map;
        Graph g;
        EdgeSparsification();

        explicit EdgeSparsification(const boost::filesystem::path& in);

        EdgeSparsification(std::size_t seed, std::size_t size, const std::string& type);

        explicit EdgeSparsification(std::vector<Point_2> & _points);

        void compute_stretch(bool stretch_analysis);

        void to_json(const boost::filesystem::path& out, bool include_points = true, bool include_edges = true);

        void print();

        std::size_t num_points() const;

        void plot(const boost::filesystem::path& out) const;

        bool insertPoint(Point_2 p);

        bool insertEdge(std::size_t s, std::size_t t);

        bool insertEdge(Point_2 s, Point_2 t);

        std::string getId() const;

        void mergeEdges(EdgeSparsification & B);

        const std::string &getInstanceName() const;

        void setInstanceName(const std::string &instanceName);

        void setSparsificationType(const std::string &sparsificationType);

        double getMaxStretch();

        double getGraphWeight();

    };

    EdgeSparsification from_json(const boost::filesystem::path& in);

}

#endif //SPARSIFICATION_EDGESPARSIFICATION_H
