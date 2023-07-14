#ifndef SPARSIFICATION_WSPD_H
#define SPARSIFICATION_WSPD_H

#include <vector>

#include "definitions/global_definitions.h"
#include "Splittree.h"
#include "SplittreeNode.h"
#include "edge_sparsification/EdgeSparsification.h"
#include "gurobi_c++.h"

namespace Sparsification::WSPD {

    class WSPD {

    public:
        WSPD(EdgeSparsification & _instance, double _s);

        void buildWSPD(const std::shared_ptr<SplittreeNode>& u, const std::shared_ptr<SplittreeNode>& v);

        bool areWellSeparated(const std::shared_ptr<SplittreeNode>& u, const std::shared_ptr<SplittreeNode>& v) const;

        void computeGraph(int variant);

        void insertFirstEdge();

        void insertShortestEdge();

        void insertEdgesMinMaxDegree();

        void insertMinDegree();

    private:
        EdgeSparsification & instance;
        std::vector<std::shared_ptr<IndexPoint>> points;
        std::vector<std::pair<std::shared_ptr<SplittreeNode>, std::shared_ptr<SplittreeNode>>> pairs;
        double s;

    };

}

#endif //SPARSIFICATION_WSPD_H