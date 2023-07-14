#ifndef SPARSIFICATION_INSTANCE_H
#define SPARSIFICATION_INSTANCE_H

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/asio/ip/host_name.hpp>

#include "edge_sparsification/EdgeSparsification.h"
#include "edge_sparsification/sparsification.h"
#include "definitions/global_definitions.h"
#include "utils/json_utils.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Solver {

    class Instance {

    public:
        std::vector<Point_2> points;
        std::string instance_name;
        std::string pointset_name;
        Graph g;

        Instance();

        explicit Instance(Sparsification::EdgeSparsification edge_sparsification);

        explicit Instance(const boost::filesystem::path& in, bool scale = false);

        json to_json(bool points_out, bool edges_out);

    };

}

#endif //SPARSIFICATION_INSTANCE_H
