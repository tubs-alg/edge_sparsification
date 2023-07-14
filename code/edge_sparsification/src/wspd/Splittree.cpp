#include "wspd/Splittree.h"

namespace Sparsification::WSPD {

    Splittree::Splittree() = default;

    Splittree::Splittree(std::vector<std::shared_ptr<IndexPoint>> & _points) {
        double xmin = std::numeric_limits<double>::infinity();
        double xmax = -std::numeric_limits<double>::infinity();
        double ymin = std::numeric_limits<double>::infinity();
        double ymax = -std::numeric_limits<double>::infinity();
        for (auto & index_p : _points) {
            if (index_p->second.x() < xmin) xmin = index_p->second.x();
            if (index_p->second.x() > xmax) xmax = index_p->second.x();
            if (index_p->second.y() < ymin) ymin = index_p->second.y();
            if (index_p->second.y() > ymax) ymax = index_p->second.y();
        }
        Bbox_2 bb = Bbox_2{xmin, ymin, xmax, ymax};
        root = constructSplittree(_points, bb);
    }

    Splittree::Splittree(std::vector<std::shared_ptr<IndexPoint>> & _points, Bbox_2 rectangle) {
        root = constructSplittree(_points, rectangle);
    }

    std::shared_ptr<SplittreeNode> Splittree::constructSplittree(std::vector<std::shared_ptr<IndexPoint>> & _points, Bbox_2 rectangle) {
        std::shared_ptr<SplittreeNode> node;
        if (_points.size() == 1) {
            node = std::make_shared<SplittreeNode>(_points.at(0), rectangle);
            return node;
        }
        node = std::make_shared<SplittreeNode>(_points, rectangle);
        int maxDim = node->getDimWithMaxSideLength();
        std::pair<Bbox_2, Bbox_2> splittedBB = node->split(maxDim);
        std::vector<std::shared_ptr<IndexPoint>> P1;
        std::vector<std::shared_ptr<IndexPoint>> P2;
        for (auto & p : _points) {
            if (CGAL::do_overlap(splittedBB.first, Bbox_2{p->second.x(), p->second.y(), p->second.x(), p->second.y()})) {
                P1.push_back(p);
            } else {
                P2.push_back(p);
            }
        }
        node->leftChild = constructSplittree(P1, splittedBB.first);
        node->rightChild = constructSplittree(P2, splittedBB.second);

        return node;
    }

    std::vector<std::shared_ptr<SplittreeNode>> Splittree::getInnerNodes() {
        std::vector<std::shared_ptr<SplittreeNode>> nodes;
        if (!root->isLeaf()) {
            nodes.emplace_back(root);
            root->getInnerNodes(nodes);
        }
        return nodes;
    }

    std::vector<std::shared_ptr<IndexPoint>> Splittree::getPoints() {
        return root->getPoints();
    }

}
