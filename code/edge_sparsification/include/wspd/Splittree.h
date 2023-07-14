#ifndef SPARSIFICATION_SPLITTREE_H
#define SPARSIFICATION_SPLITTREE_H

#include <vector>
#include "definitions/global_definitions.h"
#include "wspd/SplittreeNode.h"

namespace Sparsification::WSPD {

    class Splittree {

    public:

        Splittree();

        Splittree(std::vector<std::shared_ptr<IndexPoint>> & _points);

        Splittree(std::vector<std::shared_ptr<IndexPoint>> & _points, Bbox_2 rectangle);

        std::vector<std::shared_ptr<SplittreeNode>> getInnerNodes();

        std::vector<std::shared_ptr<IndexPoint>> getPoints();
    private:

        std::shared_ptr<SplittreeNode> root = nullptr;
        std::shared_ptr<SplittreeNode> constructSplittree(std::vector<std::shared_ptr<IndexPoint>> & _points, Bbox_2 rectangle);
    };

}

#endif //SPARSIFICATION_SPLITTREE_H