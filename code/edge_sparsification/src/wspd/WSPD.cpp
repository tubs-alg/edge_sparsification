#include "wspd/WSPD.h"

namespace Sparsification::WSPD {

    WSPD::WSPD( EdgeSparsification & _instance, double _s) : instance(_instance) {
        std::size_t n = 0;
        for (auto & p : instance.points) {
            points.emplace_back(std::make_shared<IndexPoint>(IndexPoint(n++, p)));
        }
        pairs = std::vector<std::pair<std::shared_ptr<SplittreeNode>, std::shared_ptr<SplittreeNode>>>();
        Splittree stree{points};
        s = _s;
        std::vector<std::shared_ptr<SplittreeNode>> innerNodes = stree.getInnerNodes();
        for (auto & node : innerNodes) {
            buildWSPD(node->leftChild, node->rightChild);
        }
//        std::cout << "Separation factor: " << s << std::endl;
//        std::cout << "WSPD Size: " << pairs.size() << std::endl;
    }

    void WSPD::buildWSPD(const std::shared_ptr<SplittreeNode>& u, const std::shared_ptr<SplittreeNode>& v) {
        if (areWellSeparated(u, v)) {
            pairs.emplace_back(std::make_pair(u, v));
            return;
        }
        if (u->getLMax() <= v->getLMax() && v->leftChild != nullptr) {
            buildWSPD(u, v->leftChild);
            buildWSPD(u, v->rightChild);
        } else if (u->leftChild != nullptr) {
            buildWSPD(u->leftChild, v);
            buildWSPD(u->rightChild, v);
        }
    }

    bool WSPD::areWellSeparated(const std::shared_ptr<SplittreeNode>& u, const std::shared_ptr<SplittreeNode>& v) const {
        double uR = u->getRadius();
        double vR = v->getRadius();
        double maxRadius = std::max(uR, vR);
        bool ans = CGAL::sqrt(CGAL::squared_distance(u->getCenter(), v->getCenter())) - (uR + vR) >= s*maxRadius;
        return ans;
    }

    void WSPD::computeGraph(int variant) {
        switch (variant) {
            case 0:
                insertFirstEdge();
                break;
            case 1:
                insertShortestEdge();
                break;
            case 2:
                insertEdgesMinMaxDegree();
                break;
            case 3:
                insertMinDegree();
                break;
            default:
                std::cout << "Variant not defined: " << variant << std::endl;
        }
    }

    void WSPD::insertFirstEdge() {
        for (auto & pair : pairs) {
            auto P1 = pair.first->getFirstPoint();
            auto P2 = pair.second->getFirstPoint();
            auto si = P1->first;
            auto ti = P2->first;
            instance.insertEdge(si, ti);
        }
    }

    void WSPD::insertShortestEdge() {
        for (auto & pair : pairs) {
            auto points1 = pair.first->getPoints();
            auto points2 = pair.second->getPoints();
            double shortest = std::numeric_limits<double>::infinity();
            std::size_t si;
            std::size_t ti;
            for (auto &p1 : points1) {
                for (auto &p2 : points2) {
                    double dist2 = CGAL::squared_distance(p1->second, p2->second);
                    if (dist2 < shortest) {
                        shortest = dist2;
                        si = p1->first;
                        ti = p2->first;
                    }
                }
            }
            instance.insertEdge(si, ti);
        }
    }

    void WSPD::insertEdgesMinMaxDegree() {
        GRBEnv *env;
        using edge = std::pair<std::size_t, std::size_t>;
        std::map<edge, GRBVar> xvars;

        try {
            env = new GRBEnv();
            GRBModel model = GRBModel(*env);

            GRBVar maxDegree = model.addVar(0, GRB_INFINITY, 1, GRB_CONTINUOUS, "maxDegree");
            /*
             * initialize all x and z variables
             */
            for (std::size_t i = 0; i < points.size(); ++i) {
                for (std::size_t j = i + 1; j < points.size(); ++j) {
                    xvars[edge(i,j)] = model.addVar(0, 1, 0, GRB_BINARY,
                                               "x_" + boost::lexical_cast<std::string>(i)
                                               + "_" + boost::lexical_cast<std::string>(j));
                    xvars[edge(j,i)] = xvars[edge(i,j)];
                }
                /*
                 * Add degree constraint
                 */
                GRBLinExpr expr = 0;
                for (std::size_t j = 0; j < points.size(); ++j) {
                    if (i == j) continue;
                    expr += xvars[edge(i,j)];
                }
                model.addConstr(expr <= maxDegree, "degree_" + boost::lexical_cast<std::string>(i));
            }

            /*
             * Every WSPD pair has to have exactly one edge
             */
            int n = 0;
            for (auto & pair : pairs) {
                GRBLinExpr expr = 0;
                auto points1 = (*(pair.first)).getPoints();
                auto points2 = (*(pair.second)).getPoints();
                for (auto &p1 : points1) {
                    for (auto &p2 : points2) {
                        auto si = p1->first;
                        auto ti = p2->first;
                        expr += xvars[edge(si,ti)];
                    }
                }
                model.addConstr(expr == 1, "wspd_" + boost::lexical_cast<std::string>(n++));
            }

            model.optimize();

            if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
                for (std::size_t i = 0; i < points.size(); ++i) {
                    for (std::size_t j = i + 1; j < points.size(); ++j) {
                        if (xvars[edge(i,j)].get(GRB_DoubleAttr_X) > 0.5) {
                            instance.insertEdge(i, j);
                        }
                    }
                }
            }
        } catch (GRBException& e) {
            std::cout << "Error during optimization" << std::endl;
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during optimization" << std::endl;
        }
        delete env;
    }

    void WSPD::insertMinDegree() {
        for (auto & pair : pairs) {
            auto points1 = pair.first->getPoints();
            auto points2 = pair.second->getPoints();
            std::size_t min_degree_points1 = boost::out_degree(points1[0]->first, instance.g);
            std::size_t min_degree_points2 = boost::out_degree(points2[0]->first, instance.g);
            std::size_t si = points1[0]->first;
            std::size_t ti = points2[0]->first;
            for (auto &p1 : points1) {
                if (boost::out_degree(p1->first, instance.g) < min_degree_points1) {
                    min_degree_points1 = boost::out_degree(p1->first, instance.g);
                    si = p1->first;
                }
            }
            for (auto &p2 : points2) {
                if (boost::out_degree(p2->first, instance.g) < min_degree_points2) {
                    min_degree_points1 = boost::out_degree(p2->first, instance.g);
                    ti = p2->first;
                }
            }
            instance.insertEdge(si, ti);
        }
    }

}
