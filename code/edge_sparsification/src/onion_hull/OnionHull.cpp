#include "onion_hull/OnionHull.h"

namespace Sparsification {

    bool sort_pair_double_size_t(const std::pair<double, std::size_t> &a, const std::pair<double, std::size_t> &b) {
        return a.first < b.first;
    }

    OnionHull::OnionHull(EdgeSparsification &_instance) : instance(_instance) {
        std::vector<std::size_t> indices(instance.points.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::vector<std::size_t> out;

        while (!indices.empty()) {

            CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                                Convex_hull_traits_2(CGAL::make_property_map(instance.points)));

            layers.emplace_back(out);
            // insert CH edges
            for (std::size_t i = 0; i < out.size(); ++i) {
                instance.insertEdge(out[i], out[(i+1) % out.size()]);
            }

            //sort ascending
            std::sort(out.begin(), out.end());
            //remove CH indices
            int i = 0;
            for (std::size_t ch_i : out) {
                while (ch_i != indices[i]) {
                    i++;
                }
                indices.erase(indices.begin() + i);
            }

            //clear the CH output for the next onion layer
            out.clear();

        }
        instance.json_s["layerCnt"] = layers.size();

    }

    void OnionHull::connectLayers(int variant, int k) {
        switch (variant) {
            case 0:
                contiguousLayersComplete();
                break;
            case 1:
                contiguousLayersDistance(k);
                break;
            case 2:
                contiguousLayersAngle(k);
                break;
            default:
                std::cout << "Variant not defined: " << variant << std::endl;
        }
    }

    void OnionHull::contiguousLayersComplete() {
        for (std::size_t i = 0; i < layers.size()-1; ++i) {
            auto layer = layers[i];
            auto succ_layer = layers[(i+1) % layers.size()];
            for (auto s : layer) {
                for (auto t : succ_layer) {
                    instance.insertEdge(s, t);
                }
            }
        }
    }

    void OnionHull::contiguousLayersDistance(std::size_t k) {
        for (std::size_t i = 0; i < layers.size(); ++i) {
            auto layer = layers[i];
            auto succ_layer = layers[(i+1) % layers.size()];
            for (auto s : layer) {
                std::vector<std::pair<double, std::size_t>> distance_index;
                for (auto t : succ_layer) {
                    distance_index.emplace_back(CGAL::squared_distance(instance.points[s], instance.points[t]), t);
                }
                std::sort(distance_index.begin(), distance_index.end(), sort_pair_double_size_t);
                for (int j = 0; j < k && j < distance_index.size(); ++j) {
                    instance.insertEdge(s, distance_index.at(j).second);
                }
            }
        }
    }

    void OnionHull::contiguousLayersAngle(std::size_t k) {
        for (int i = 0; i < layers.size(); ++i) {
            auto layer = layers[i];
            auto succ_layer = layers[(i+1) % layers.size()];
            for (std::size_t s_i = 0; s_i < layer.size(); ++s_i) {
                std::vector<std::pair<double, std::size_t>> angle_index;
                auto s = layer[s_i];

                std::size_t pred_i = (s_i + layer.size() - 1) % layer.size();
                auto pred = layer[pred_i];
                Vector_2 a{instance.points[pred], instance.points[s]};
                for (auto t : succ_layer) {
                    Vector_2 b{instance.points[s], instance.points[t]};
                    double angle = getAngle(a, b);
                    angle_index.emplace_back(angle, t);
                }
                std::sort(angle_index.begin(), angle_index.end(), sort_pair_double_size_t);
                for (std::size_t j = 0; j < k && j < angle_index.size(); ++j) {
                    instance.insertEdge(s, angle_index.at(j).second);
                }
                angle_index.clear();

                std::size_t succ_i = (s_i + layer.size() + 1) % layer.size();
                auto succ = layer[succ_i];
                a = Vector_2{instance.points[succ], instance.points[s]};
                for (auto t : succ_layer) {
                    Vector_2 b{instance.points[s], instance.points[t]};
                    double angle = getAngle(a, b);
                    angle_index.emplace_back(angle, t);
                }
                std::sort(angle_index.begin(), angle_index.end(), sort_pair_double_size_t);
                for (int j = 0; j < k && j < angle_index.size(); ++j) {
                    instance.insertEdge(s, angle_index[j].second);
                }
            }
        }
    }

    double OnionHull::getAngle(Vector_2 & a, Vector_2 & b) {
        double angle;
        if (CGAL::orientation(a, b) == CGAL::COLLINEAR) {
            if (a.direction() == b.direction()) {
                angle = 0.0;
            } else {
                angle = 3.141592653589;
            }
        } else {
            angle = std::acos(a * b / (CGAL::sqrt(a.squared_length()) *
                                       CGAL::sqrt(b.squared_length())));
        }
        return angle;
    }

}