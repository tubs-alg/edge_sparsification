#ifndef MWT_STATIC_QUAD_TREE_H
#define MWT_STATIC_QUAD_TREE_H

#include <iterator>
#include <algorithm>
#include <vector>
#include <memory>
#include <array>

#include <CGAL/Bbox_2.h>
#include <CGAL/enum.h>

namespace CGAL {

    namespace Minimum_weight_triangulation {


        //--------------------------------------------------------------------------------------------------------------
        // Some helper enums and structs that are used to Hilbert sort the point set while building the quad tree
        //--------------------------------------------------------------------------------------------------------------
        enum class Order {
            XY,
            YX
        };

        enum class Dimension {
            X,
            Y
        };

        template <Order >
        struct Split;

        template <>
        struct Split<Order::XY> {
            static constexpr Order same = Order::XY;
            static constexpr Order other = Order::YX;
        };

        template <>
        struct Split<Order::YX> {
            static constexpr Order same = Order::YX;
            static constexpr Order other = Order::XY;
        };

        template <class K, Dimension d, bool reverse_order> struct Fixed_value_cmp_2;

        template <class K, Dimension d>
        struct Fixed_value_cmp_2<K, d, true>
        {
            using FT = typename K::FT;
            using Point_2 = typename K::Point_2;

            FT value;

            Fixed_value_cmp_2 (const FT& v) : value(v) {}

            bool operator() (const Point_2 &p) const { return !Fixed_value_cmp_2<K, d, false>{value}(p); }
        };

        template <class K>
        struct Fixed_value_cmp_2<K, Dimension::X, false>
        {
            using FT = typename K::FT;
            using Point_2 = typename K::Point_2;

            FT value;

            Fixed_value_cmp_2 (const FT& v) : value(v) {}
            bool operator() (const Point_2 &p) const { return p.x() < value; }
        };

        template <class K>
        struct Fixed_value_cmp_2<K, Dimension::Y, false>
        {
            using FT = typename K::FT;
            using Point_2 = typename K::Point_2;

            FT value;

            Fixed_value_cmp_2 (const FT& v) : value(v) {}

            bool operator() (const Point_2 &p) const { return p.y() < value; }
        };
        //--------------------------------------------------------------------------------------------------------------



        // Node order in the quad tree
        //  ___________
        // |     |     |
        // |  1  |  2  |
        // |_____|_____|
        // |     |     |
        // |  0  |  3  |
        // |_____|_____|
        //
        template<typename Traits_, typename Iterator_>
        struct Quad_tree_node {
            using Traits = Traits_;
            using Iterator = Iterator_;

            using FT = typename Traits::FT;
            using Point_2 = typename Traits::Point_2;
            using Iso_rectangle_2 = typename Traits::Iso_rectangle_2;

            const Iso_rectangle_2 bbox;
            Quad_tree_node *children;
            const Iterator begin;
            const Iterator end;

            Quad_tree_node(const Bbox_2& box, Iterator begin, Iterator end)
                    : bbox{box}, children{nullptr}, begin{begin}, end{end} {

                CGAL_expensive_assertion(
                        std::none_of(begin, end, [&](const Point_2& p) {return bbox.has_on_unbounded_side(p);})
                );
            }

            Quad_tree_node(const FT &xmin, const FT &ymin,
                           const FT &xmax, const FT &ymax,
                           Iterator begin, Iterator end)
                    : bbox{xmin, ymin, xmax, ymax}, children{nullptr}, begin{begin}, end{end} {

                CGAL_expensive_assertion(
                        std::none_of(begin, end, [&](const Point_2& p) {return bbox.has_on_unbounded_side(p);})
                );
            }

            bool is_leaf() const {
                return children == nullptr;
            }

            bool is_empty() const {
                return begin == end;
            }

            const Iso_rectangle_2& bounding_box() const {
                return bbox;
            }

            Iterator points_begin() const {
                return begin;
            }

            Iterator points_end() const {
                return end;
            }

            const Quad_tree_node* children_begin() const {
                return children;
            }

            const Quad_tree_node* children_end() const {
                return children + 4;
            }


            FT squared_distance(const Point_2& p) const {
                FT distance{0};
                if (p.x() < bbox.xmin())
                    distance += (bbox.xmin() - p.x()) * (bbox.xmin() - p.x());
                else if (p.x() > bbox.xmax())
                    distance += (p.x() - bbox.xmax()) * (p.x() - bbox.xmax());

                if (p.y() < bbox.ymin())
                    distance += (bbox.ymin() - p.y()) * (bbox.ymin() - p.y());
                else if (p.y() > bbox.ymax())
                    distance += (p.y() - bbox.ymax()) * (p.y() - bbox.ymax());

                return distance;


//                FT a = std::max(bbox.xmin() - p.x(), FT{0});
//                FT b = std::max(p.x() - bbox.xmax(), FT{0});
//                FT c = std::max(bbox.ymin() - p.y(), FT{0});
//                FT d = std::max(p.y() - bbox.ymax(), FT{0});
//
//                return a*a + b*b + c*c + d*d;

            }

            std::array<FT, 4> squared_distance_to_children(const Point_2& p) const {
                return squared_distance_to_children(p, false);
            };


            std::array<FT, 4> squared_distance_to_children(const Point_2& p, bool p_inside_bbox) const {
                CGAL_precondition(children != nullptr);

                if(p_inside_bbox) {
                    return squared_distance_to_children_inside_bbox(p);
                }
                else {

                    std::array<FT, 4> distance{0, 0, 0, 0};

                    distance[0] = children[0].squared_distance(p);
                    distance[1] = children[1].squared_distance(p);
                    distance[2] = children[2].squared_distance(p);
                    distance[3] = children[3].squared_distance(p);

                    return distance;
                }
            }


        private:
            std::array<FT, 4> squared_distance_to_children_inside_bbox(const Point_2& p) const {

                CGAL_precondition(!bbox.has_on_unbounded_side(p));

                std::array<FT, 4> distance{0,0,0,0};

                const FT mx = FT{0.5} * (bbox.xmin() + bbox.xmax());
                const FT my = FT{0.5} * (bbox.ymin() + bbox.ymax());

                const FT dx = (p.x() - mx) * (p.x() - mx);
                const FT dy = (p.y() - my) * (p.y() - my);
                const FT dxdy = dx + dy;

                if(p.x() < mx) {
                    if(p.y() < my) {
                        distance[1] = dy;
                        distance[2] = dxdy;
                        distance[3] = dx;
                    } else {
                        distance[0] = dy;
                        distance[2] = dx;
                        distance[3] = dxdy;
                    }
                }
                else {
                    if(p.y() < my) {
                        distance[0] = dx;
                        distance[1] = dxdy;
                        distance[2] = dy;
                    } else {
                        distance[0] = dxdy;
                        distance[1] = dx;
                        distance[3] = dy;
                    }
                }

                CGAL_expensive_postcondition(distance[0] == children[0].squared_distance(p));
                CGAL_expensive_postcondition(distance[1] == children[1].squared_distance(p));
                CGAL_expensive_postcondition(distance[2] == children[2].squared_distance(p));
                CGAL_expensive_postcondition(distance[3] == children[3].squared_distance(p));


                return distance;
            }
        };




        template<typename Traits_, typename RandomAccessIterator>
        class Static_quad_tree {
        public:
            using Traits = Traits_;
            using Iterator = RandomAccessIterator;

            using FT = typename Traits::FT;
            using Point_2 = typename Traits::Point_2;

            using Node = Quad_tree_node<Traits, Iterator>;
            using Node_allocator = std::allocator<Node>;

            Static_quad_tree(Iterator begin, Iterator end)
                    : points_begin{begin}, points_end{end}, node_allocator{}, root_node{nullptr} {

                build_tree(begin, end);
            };

            ~Static_quad_tree() {

                if(root_node != nullptr) {
                    deallocate_children(root_node);
                    node_allocator.destroy(root_node);
                    node_allocator.deallocate(root_node, 1);
                }
            }


            void verify_tree() {
                verify_node(root_node);
            }

            void verify_node(const Node* node) {

                CGAL_assertion(
                        std::none_of(node->begin, node->end, [&](const Point_2& p) {return node->bbox.has_on_unbounded_side(p);})
                );

                if(node->is_leaf()) {
                    CGAL_assertion(std::distance(node->begin, node->end) < split_threshold);

                }
                else {
                  for(int i{0}; i < 4; ++i) {
                      verify_node(node->children + i);
                  }
                }
            }


            bool is_empty() const {
                return root_node == nullptr;
            }

            const Node* root() const {
                return root_node;
            }

            Iterator begin() const {
                return points_begin;
            }

            Iterator end() const {
                return points_end;
            }

        private:

            void deallocate_children(Node* node) {

                if(node->children == nullptr)
                    return;

                deallocate_children(node->children + 0);
                deallocate_children(node->children + 1);
                deallocate_children(node->children + 2);
                deallocate_children(node->children + 3);

                node_allocator.destroy(node->children + 0);
                node_allocator.destroy(node->children + 1);
                node_allocator.destroy(node->children + 2);
                node_allocator.destroy(node->children + 3);

                node_allocator.deallocate(node->children, 4);

            }

            void build_tree(Iterator begin, Iterator end) {

                if(begin == end)
                    return;

                auto bbox = bbox_2(begin, end);

                root_node = node_allocator.allocate(1);
                node_allocator.construct(root_node, bbox, begin, end);

                hilbert_split_node<Order::XY, false>(root_node);

            }

            template <Order order, bool reverse>
            void hilbert_split_node(Node *node) {


                if(std::distance(node->begin, node->end) < split_threshold)
                    return;

                const FT xmin = node->bounding_box().xmin();
                const FT ymin = node->bounding_box().ymin();
                const FT xmax = node->bounding_box().xmax();
                const FT ymax = node->bounding_box().ymax();

                const FT xmid = FT{0.5} * (xmin + xmax);
                const FT ymid = FT{0.5} * (ymin + ymax);


                Iterator m0 = node->begin;
                Iterator m4 = node->end;

                Iterator m1, m2, m3;

                if(order == Order::XY) {
                    m2 = std::partition(m0, m4, Fixed_value_cmp_2<Traits, Dimension::X, reverse> {xmid});
                    m1 = std::partition(m0, m2, Fixed_value_cmp_2<Traits, Dimension::Y, reverse> {ymid});
                    m3 = std::partition(m2, m4, Fixed_value_cmp_2<Traits, Dimension::Y, !reverse> {ymid});
                }
                if(order == Order::YX) {
                    m2 = std::partition(m0, m4, Fixed_value_cmp_2<Traits, Dimension::Y, reverse> {ymid});
                    m1 = std::partition(m0, m2, Fixed_value_cmp_2<Traits, Dimension::X, reverse> {xmid});
                    m3 = std::partition(m2, m4, Fixed_value_cmp_2<Traits, Dimension::X, !reverse> {xmid});
                }

                node->children = node_allocator.allocate(4);

                if(order == Order::XY && !reverse) {
                    node_allocator.construct(node->children + 0, xmin, ymin, xmid, ymid, m0, m1);
                    node_allocator.construct(node->children + 1, xmin, ymid, xmid, ymax, m1, m2);
                    node_allocator.construct(node->children + 2, xmid, ymid, xmax, ymax, m2, m3);
                    node_allocator.construct(node->children + 3, xmid, ymin, xmax, ymid, m3, m4);

                    hilbert_split_node<Split<order>::other, reverse> (node->children + 0);
                    hilbert_split_node<Split<order>::same, reverse> (node->children + 1);
                    hilbert_split_node<Split<order>::same, reverse> (node->children + 2);
                    hilbert_split_node<Split<order>::other,!reverse> (node->children + 3);
                }
                if(order == Order::XY && reverse) {
                    node_allocator.construct(node->children + 0, xmin, ymin, xmid, ymid, m2, m3);
                    node_allocator.construct(node->children + 1, xmin, ymid, xmid, ymax, m3, m4);
                    node_allocator.construct(node->children + 2, xmid, ymid, xmax, ymax, m0, m1);
                    node_allocator.construct(node->children + 3, xmid, ymin, xmax, ymid, m1, m2);

                    hilbert_split_node<Split<order>::same, reverse> (node->children + 0);
                    hilbert_split_node<Split<order>::other, !reverse> (node->children + 1);
                    hilbert_split_node<Split<order>::other, reverse> (node->children + 2);
                    hilbert_split_node<Split<order>::same, reverse> (node->children + 3);
                }
                if(order == Order::YX && !reverse) {
                    node_allocator.construct(node->children + 0, xmin, ymin, xmid, ymid, m0, m1);
                    node_allocator.construct(node->children + 1, xmin, ymid, xmid, ymax, m3, m4);
                    node_allocator.construct(node->children + 2, xmid, ymid, xmax, ymax, m2, m3);
                    node_allocator.construct(node->children + 3, xmid, ymin, xmax, ymid, m1, m2);

                    hilbert_split_node<Split<order>::other, reverse> (node->children + 0);
                    hilbert_split_node<Split<order>::other, !reverse> (node->children + 1);
                    hilbert_split_node<Split<order>::same, reverse> (node->children + 2);
                    hilbert_split_node<Split<order>::same, reverse> (node->children + 3);
                }
                if(order == Order::YX && reverse) {
                    node_allocator.construct(node->children + 0, xmin, ymin, xmid, ymid, m2, m3);
                    node_allocator.construct(node->children + 1, xmin, ymid, xmid, ymax, m1, m2);
                    node_allocator.construct(node->children + 2, xmid, ymid, xmax, ymax, m0, m1);
                    node_allocator.construct(node->children + 3, xmid, ymin, xmax, ymid, m3, m4);

                    hilbert_split_node<Split<order>::same, reverse> (node->children + 0);
                    hilbert_split_node<Split<order>::same, reverse> (node->children + 1);
                    hilbert_split_node<Split<order>::other, reverse> (node->children + 2);
                    hilbert_split_node<Split<order>::other, !reverse> (node->children + 3);
                }

            }


            void simple_split_node(Node* node) {

                if(std::distance(node->begin, node->end) < split_threshold)
                    return;


                const FT xmin = node->bounding_box().xmin();
                const FT ymin = node->bounding_box().ymin();
                const FT xmax = node->bounding_box().xmax();
                const FT ymax = node->bounding_box().ymax();

                const FT xmid = FT{0.5} * (xmin + xmax);
                const FT ymid = FT{0.5} * (ymin + ymax);


                Iterator m0 = node->begin;
                Iterator m4 = node->end;

                Iterator m1, m2, m3;

                m2 = std::partition(m0, m4, Fixed_value_cmp_2<Traits, Dimension::X, false> {xmid});
                m1 = std::partition(m0, m2, Fixed_value_cmp_2<Traits, Dimension::Y, false> {ymid});
                m3 = std::partition(m2, m4, Fixed_value_cmp_2<Traits, Dimension::Y, true> {ymid});


                node->children = node_allocator.allocate(4);

                node_allocator.construct(node->children + 0, xmin, ymin, xmid, ymid, m0, m1);
                node_allocator.construct(node->children + 1, xmin, ymid, xmid, ymax, m1, m2);
                node_allocator.construct(node->children + 2, xmid, ymid, xmax, ymax, m2, m3);
                node_allocator.construct(node->children + 3, xmid, ymin, xmax, ymid, m3, m4);

                simple_split_node(node->children + 0);
                simple_split_node(node->children + 1);
                simple_split_node(node->children + 2);
                simple_split_node(node->children + 3);

            }


            const int split_threshold = 16;
            const Iterator points_begin;
            const Iterator points_end;

            Node_allocator node_allocator;

            Node *root_node;

        };

    }
}


#endif //MWT_STATIC_QUAD_TREE_H
