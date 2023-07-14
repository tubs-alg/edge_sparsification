#ifndef SPARSIFICATION_SPLITTREENODE_H
#define SPARSIFICATION_SPLITTREENODE_H

#include "definitions/global_definitions.h"

namespace Sparsification::WSPD {

    class SplittreeNode {

    private:
        int level = 0;
        int size = 0;
        Bbox_2 R;
        Bbox_2 minBB;

    public:

        std::shared_ptr<SplittreeNode> leftChild = nullptr;
        std::shared_ptr<SplittreeNode> rightChild = nullptr;
        std::shared_ptr<IndexPoint> point = nullptr;
        std::vector<std::shared_ptr<IndexPoint>> points;

        SplittreeNode();

        SplittreeNode(std::shared_ptr<IndexPoint> & _p, Bbox_2 & rectangle);

        SplittreeNode(std::vector<std::shared_ptr<IndexPoint>> & _points, Bbox_2 & rectangle);

        int getDimWithMaxSideLength();

        std::pair<Bbox_2, Bbox_2> split(int dimToSplit);

        std::shared_ptr<IndexPoint> getFirstPoint();

        std::vector<std::shared_ptr<IndexPoint>> getPoints();

        void getPoints(std::vector<std::shared_ptr<IndexPoint>> & points);

        bool isLeaf() const;

        Point_2 getCenter();

        double getRadius();

        Point_2 getCorner(int i);

        double getLMax();

        void getInnerNodes(std::vector<std::shared_ptr<SplittreeNode>> &v);

    };

}

#endif //SPARSIFICATION_SPLITTREENODE_H
