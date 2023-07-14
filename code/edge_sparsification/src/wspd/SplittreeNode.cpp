#include "wspd/SplittreeNode.h"

namespace Sparsification::WSPD {

    SplittreeNode::SplittreeNode() = default;

    SplittreeNode::SplittreeNode(std::shared_ptr<IndexPoint> & _p, Bbox_2 & rectangle) {
        R = rectangle;
        point = _p;
        points.push_back(point);
        size = 1;
        minBB = Bbox_2{_p->second.x(), _p->second.y(), _p->second.x(), _p->second.y()};
    }

    SplittreeNode::SplittreeNode(std::vector<std::shared_ptr<IndexPoint>> & _points, Bbox_2 & rectangle) {
        R = rectangle;
        points = _points;
        double xmin = std::numeric_limits<double>::infinity();
        double xmax = -std::numeric_limits<double>::infinity();
        double ymin = std::numeric_limits<double>::infinity();
        double ymax = -std::numeric_limits<double>::infinity();
        for (auto & index_p : points) {
            if (index_p->second.x() < xmin) xmin = index_p->second.x();
            if (index_p->second.x() > xmax) xmax = index_p->second.x();
            if (index_p->second.y() < ymin) ymin = index_p->second.y();
            if (index_p->second.y() > ymax) ymax = index_p->second.y();
        }
        minBB = Bbox_2{xmin, ymin, xmax, ymax};
        size = points.size();
    }

    int SplittreeNode::getDimWithMaxSideLength() {
        if ((minBB.max(0) - minBB.min(0)) > (minBB.max(1) - minBB.min(1))) {
            return 0;
        }
        return 1;
    }

    std::pair<Bbox_2, Bbox_2> SplittreeNode::split(int dimToSplit) {
        Bbox_2 R1;
        Bbox_2 R2;
        double center =  (minBB.max(dimToSplit) + minBB.min(dimToSplit)) / 2;
        if (dimToSplit) {
            R1 = Bbox_2{R.min(0), R.min(1), R.max(0), center};
            R2 = Bbox_2{R.min(0), center, R.max(0), R.max(1)};
        } else {
            R1 = Bbox_2{R.min(0), R.min(1), center, R.max(1)};
            R2 = Bbox_2{center, R.min(1), R.max(0), R.max(1)};
        }
        return std::make_pair(R1,R2);
    }

    std::shared_ptr<IndexPoint> SplittreeNode::getFirstPoint() {
        if (leftChild == nullptr) {
            return point;
        }
        return leftChild->getFirstPoint();
    }

    std::vector<std::shared_ptr<IndexPoint>> SplittreeNode::getPoints() {
        return points;
    }

    void SplittreeNode::getPoints(std::vector<std::shared_ptr<IndexPoint>> & points) {
        if (leftChild == nullptr) {
            points.emplace_back(point);
        } else {
            leftChild->getPoints(points);
            rightChild->getPoints(points);
        }
    }

    bool SplittreeNode::isLeaf() const {
        return (leftChild == nullptr && rightChild == nullptr);
    }

    Point_2 SplittreeNode::getCenter() {
        return CGAL::centroid(getCorner(0), getCorner(1), getCorner(2), getCorner(3));
    }

    double SplittreeNode::getRadius() {
        return CGAL::sqrt(CGAL::squared_distance(getCenter(), getCorner(0)));
    }

    Point_2 SplittreeNode::getCorner(int i) {
        assert (i >= 0 && i <= 3);
        switch (i) {
            case 0:
                return Point_2{minBB.min(0), minBB.min(1)};
            case 1:
                return Point_2{minBB.max(0), minBB.min(1)};
            case 2:
                return Point_2{minBB.max(0), minBB.max(1)};
            default:
                return Point_2{minBB.min(0), minBB.max(1)};
        }
    }

    double SplittreeNode::getLMax() {
        if ((minBB.max(0) - minBB.min(0)) > (minBB.max(1) - minBB.min(1))) {
            return (minBB.max(0) - minBB.min(0));
        }
        return (minBB.max(1) - minBB.min(1));
    }

    void SplittreeNode::getInnerNodes(std::vector<std::shared_ptr<SplittreeNode>> &v) {
        if (leftChild != nullptr) {
            if (!leftChild->isLeaf()) v.emplace_back(leftChild);
            if (!rightChild->isLeaf()) v.emplace_back(rightChild);
            leftChild->getInnerNodes(v);
            rightChild->getInnerNodes(v);
        }
    }

}
