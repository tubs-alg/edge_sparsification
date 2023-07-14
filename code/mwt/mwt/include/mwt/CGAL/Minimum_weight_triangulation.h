#ifndef MWT_MINIMUM_WEIGHT_TRIANGULATION_2_H
#define MWT_MINIMUM_WEIGHT_TRIANGULATION_2_H

#include <set>
#include <vector>
#include <iterator>
#include <cstdint>
#include <limits>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>

#include <boost/optional.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/range/join.hpp>
#include <algcplex/cplex.hpp>

#ifdef CGAL_LINKED_WITH_TBB
#include <tbb/tbb.h>
#include <thread>
#endif

#include <CGAL/tags.h>
#include <CGAL/Unique_hash_map.h>

#include "Static_quad_tree.h"
#include "Filtered_incremental_search.h"
#include "Directional_filter.h"
#include "Lmt_halfedge.h"
#include "Mwt_stats.h"

#include "mwt/CPLEX/empty_triangle.hpp"
#include "mwt/CPLEX/basic_mwt_solver.hpp"

#include "mwt/utils/vector_utils.hpp"
#include "mwt/utils/progress_bar.h"
#include "mwt/utils/ip_solver_utils.h"
#include "profiling.h"

//TODO: An error occurs when a face with holes has a boundary edge which is part of the convex hull. If so, a wrong face gets collected!

namespace CGAL {

    template<typename Traits, typename Concurrency_tag = Sequential_tag>
    class Minimum_weight_triangulation_2 {

    public:

        int simple_faces_count = 0;
        int faces_w_holes_count = 0;
        std::mutex ip_solver_mutex;

        template<typename InputIterator>
        Minimum_weight_triangulation_2(InputIterator begin,
                InputIterator end,
                bool delayed_construction = false,
                bool triangulate_simple_faces = true,
                bool triangulate_faces_with_holes = true,
                bool cplex_logging = true)
                :
                triangulate_simple_faces(triangulate_simple_faces),
                triangulate_faces_with_holes(triangulate_faces_with_holes),
                cplex_logging(cplex_logging),
                points{begin, end}
        {
            if(!delayed_construction)
                construct();
        }

        void only_diamond_test() {
            #ifdef USING_STATS
            stats.Diamond_test.time.total = measure_time<std::chrono::nanoseconds>([&] {
                diamond_filter();
            });
            #else
            diamond_filter();
            #endif
        }

        void diamond_and_lmt() {
            #ifdef USING_STATS
            stats.Diamond_test.time.total = measure_time<std::chrono::nanoseconds>([&] {
                diamond_filter();
            });

            stats.Lmt_skeleton.time.total = measure_time<std::chrono::nanoseconds>([&] {
                construct_lmt_skeleton();
            });
            #else
            diamond_filter();
            construct_lmt_skeleton();
            #endif
        }

        void construct () {
            #ifdef USING_STATS
            stats.Diamond_test.time.total = measure_time<std::chrono::nanoseconds>([&] {
                diamond_filter();
            });

            stats.Lmt_skeleton.time.total = measure_time<std::chrono::nanoseconds>([&] {
                construct_lmt_skeleton();
            });

            stats.Triangulation.time.total = measure_time<std::chrono::nanoseconds>([&] {
                collect_and_triangulate_faces();
            });
            #else
            diamond_filter();
            construct_lmt_skeleton();
            collect_and_triangulate_faces();

            #endif

            stats.Mwt_size = skeleton.size() + face_triangulation_edges.size();

            if(triangulate_simple_faces && triangulate_faces_with_holes)
            {
                size_t expected_mwt_size = 3 * points.size() - 3 - convex_hull_size;

                if(expected_mwt_size - stats.Mwt_size > 0) {
                    std::cout << expected_mwt_size - stats.Mwt_size << " edges missing. " << expected_mwt_size << " " << stats.Mwt_size << std::endl;
                }
            }
        };

        const Mwt_stats& statistics() const {
            return stats;
        }


        template <typename OutputIterator>
        OutputIterator output_mwt(OutputIterator out) const {
            for (const auto &edge : skeleton) {
                out++ = std::make_pair(edge->source(), edge->target());
            }
            for (const auto &edge : face_triangulation_edges) {
                out++ = std::make_pair(edge->source(), edge->target());
            }

            return out;
        }


        template <typename OutputIterator>
        OutputIterator output_lmt_skeleton(OutputIterator out) const {
            for (const auto &edge : skeleton) {
                out++ = std::make_pair(edge->source(), edge->target());
            }

            return out;
        }

        template <typename OutputIterator>
        OutputIterator output_candidates(OutputIterator out) const {
            for (const auto &edge : candidates) {
                out++ = std::make_pair(edge->source(), edge->target());
            }

            return out;
        }

        template <typename OutputIterator>
        OutputIterator output_diamond_edges(OutputIterator out) const {
            for (const auto &edge : diamond_edges) {
                out++ = std::make_pair(points[edge.first], points[edge.second]);
            }

            return out;
        }

        struct Face {
            using Halfedge = Lmt_halfedge<Traits>;
            using Halfedge_handle = Halfedge*;

            using Point_2 = typename Traits::Point_2;
            using Point_const_handle = const Point_2*;

            using Edge_container_type = std::vector<Halfedge_handle>;
            using Vertex_container_type = std::vector<Point_const_handle>;

            Edge_container_type boundary;
            Edge_container_type inner_edges;
            Edge_container_type hole_boundaries;
            Vertex_container_type isolated_vertices;

            void clear() {
                boundary.clear();
                inner_edges.clear();
                hole_boundaries.clear();
                isolated_vertices.clear();
            }

            void reserve() {
                boundary.reserve(64);
                inner_edges.reserve(128);
                hole_boundaries.reserve(64);
                isolated_vertices.reserve(16);
            }

            bool has_inner_edges() const {
                return !inner_edges.empty();
            }

            bool is_simple() const {
                return hole_boundaries.empty() && isolated_vertices.empty();
            }
        };

        void register_simple_face_callback(std::function<void(Face&)> callback)
        {
            simple_face_callback = callback;
        }

        void register_face_with_holes_callback(std::function<void(Face &)> callback)
        {
            face_with_holes_callback = callback;
        }

        void register_pre_ip_solve_callback(std::function<void(IloEnv*, IloModel*, IloCplex*, IloBoolVarArray*, unsigned int, unsigned int)> callback)
        {
            pre_ip_solve_callback = callback;
        }

        void register_post_ip_solve_callback(std::function<void(IloEnv*, IloModel*, IloCplex*, IloBoolVarArray*, unsigned int)> callback)
        {
            post_ip_solve_callback = callback;
        }

        ~Minimum_weight_triangulation_2()
        {

        }


    private:


        using FT = typename Traits::FT;
        using Point_2 = typename Traits::Point_2;
        using Segment_2 = typename Traits::Segment_2;
        using Triangle_2 = typename Traits::Triangle_2;

        using Less_xy_2 = typename Traits::Less_xy_2;
        using Left_turn_2 = typename Traits::Left_turn_2;
        using Angle_less_2 = typename Traits::Angle_less_2;

        using Construct_dead_sector_2 = typename Traits::Construct_dead_sector_2;
        using Sector_2 = typename Construct_dead_sector_2::Sector_2;
        using Sector_distance_pair = std::pair<Sector_2, FT>;

        using Compute_polar_angle_2 = typename Traits::Compute_polar_angle_2;
        using Compute_squared_distance_2 = typename Traits::Compute_squared_distance_2;

        using Point_container = std::vector<Point_2>;
        using Point_iterator = typename Point_container::iterator;
        using Point_const_iterator = typename Point_container::const_iterator;
        using Point_const_handle = const Point_2*;

        using Point_index = std::uint32_t;

        using Tree = Minimum_weight_triangulation::Static_quad_tree<Traits, Point_iterator>;
        using Search = Minimum_weight_triangulation::Filtered_incremental_search<Tree, Directional_filter>;


        struct Point_angle_pair {
            Point_const_iterator point;
            FT polar_angle;

            Point_angle_pair() = default;
            Point_angle_pair(Point_const_iterator p, FT angle) : point{p}, polar_angle{angle} {}

            bool operator < (const Point_angle_pair& rhs) const { return polar_angle < rhs.polar_angle; }
            bool operator <= (const Point_angle_pair& rhs) const { return polar_angle <= rhs.polar_angle; }
        };

        using Set = boost::container::flat_set<Point_angle_pair>;
        using Queue = std::deque<Sector_distance_pair>;


        using Edge = std::pair<Point_index , Point_index>;
        using Edge_vector = std::vector<Edge>;

        #ifdef CGAL_LINKED_WITH_TBB
        template <typename T>
        using Concurrent_vector = typename std::conditional<
                boost::is_convertible<Concurrency_tag, Parallel_tag>::value,
                tbb::concurrent_vector<T>,
                std::vector<T>>::type;
        #else
        template <typename T>
        using Concurrent_vector = std::vector<T>;
        #endif

        using Edge_container = Concurrent_vector<Edge>;


        bool triangulate_simple_faces;
        bool triangulate_faces_with_holes;
        bool cplex_logging;

        struct Diamond_filter {
            constexpr static uint expected_growth_factor = 13;
        };


        Mwt_stats stats;
        size_t convex_hull_size = 0;
        //--------------------------------------------------------------------------------------------------------------
        // Input point set
        //--------------------------------------------------------------------------------------------------------------
        Point_container points;

        //--------------------------------------------------------------------------------------------------------------
        // Spatial search tree and result container for the diamond filter
        //--------------------------------------------------------------------------------------------------------------
        std::unique_ptr<Tree> tree;
        Edge_container diamond_edges;



        //--------------------------------------------------------------------------------------------------------------
        // LMT-Skeleton members
        //--------------------------------------------------------------------------------------------------------------

        using Halfedge = Lmt_halfedge<Traits>;
        using Halfedge_handle = Halfedge*;

        // We store all halfedges in a single array (compressed sparse row graph)
        // Outgoing edges of vertex i start at edges[offsets[i]]. They are radially sorted around vertex i.
        std::vector<Halfedge> lmt_edges;
        std::vector<uint> edge_offsets;

        std::vector<Halfedge_handle> skeleton;
        std::vector<Halfedge_handle> candidates;

        //--------------------------------------------------------------------------------------------------------------
        // Intermediate result container for the dynamic programming step (triangulation of faces)
        //--------------------------------------------------------------------------------------------------------------
        Concurrent_vector<Halfedge_handle> face_triangulation_edges;



        Point_index index_of(Point_const_iterator i) const {
            CGAL_assertion(points.begin() <= i && i <= points.end());
            return static_cast<Point_index>(std::distance(points.begin(), i));
        }

        Point_index index_of(const Point_2* p) const {
            return static_cast<Point_index>(std::distance(&points.front(), p));
        }


        //--------------------------------------------------------------------------------------------------------------
        // Functions related to the diamond filter.
        //--------------------------------------------------------------------------------------------------------------

        void diamond_filter() {
            #ifdef USING_STATS
            stats.Diamond_test.time.tree_build = measure_time<std::chrono::nanoseconds>([&] {
                tree = std::make_unique<Tree>(points.begin(), points.end());
            });
            #else
            tree = std::make_unique<Tree>(points.begin(), points.end());
            #endif

            diamond_edges.reserve(Diamond_filter::expected_growth_factor * points.size());
            diamond_filter(Concurrency_tag{});

            tree.reset();

            #ifdef USING_STATS
            stats.Diamond_test.candidates_after = diamond_edges.size();
            #endif
        }

        void diamond_filter(Sequential_tag) {
            apply_diamond_filter(points.begin(), points.end(), diamond_edges);
        }

        #ifdef CGAL_LINKED_WITH_TBB
        void diamond_filter(Parallel_tag) {
            tbb::parallel_for( tbb::blocked_range<Point_const_iterator>(points.begin(), points.end(), 100),
                               [&](const tbb::blocked_range<Point_const_iterator>& r) {
                                   Edge_vector vec;
                                   vec.reserve(Diamond_filter::expected_growth_factor * r.size());

                                   apply_diamond_filter(r.begin(), r.end(), vec);

                                   diamond_edges.grow_by(vec.begin(), vec.end());
                               }
            );
        }
        #endif // CGAL_LINKED_WITH_TBB


        void apply_diamond_filter(Point_const_iterator begin, Point_const_iterator end, Edge_vector& out) {
            Search search(*tree);

            // The set stores the nearest neighbors of the query point that are found so far.
            // (Sorted by polar angle w.r.t. current query point)
            Set set{};

            // Sectors (intervals of polar angles) for which all points are known to fail the diamond test.
            // A sector is not immediately valid on creation. The distance indicates when a sector can safely be
            // inserted in the search filter.
            Queue dead_sectors;

            // For each point generate all edges that pass the diamond test. (Some additional edges might pass as well)
            for (auto query = begin; query != end; ++query) {
                auto dt = Traits{}.diamond_test_2_object();
                auto less_xy_2 = Less_xy_2{};
                auto compute_polar_angle_2 = Compute_polar_angle_2{*query};

                set.clear();
                dead_sectors.clear();

                search.search(*query);

                // Because we generate the edges for every point in the set, we only need to generate edges (q,t)
                // with lexicographically larger target, i.e. q < t. However, points in the left halfspace may be needed
                // to eliminate edges in the right halfspace. Thus we search part of the left halfspace.

                // We use pseudo polar angles that are faster to compute than the correct angle in radians.
                // [0, pi] is mapped to [0,2] and (pi, 2pi) to [-2, 0)

                //                         1.5      1        0.5
                //                                  |
                //                                  |Origin
                //  Discontinuity: 2 -> -2  ------- O ------- 0
                //                                  |
                //                                  |
                //                        -1.5     -1        -0.5


                //  ~~~~~~~~~~~~  1.25 \***
                //  ~~~~~~~~~~~~~~~~~~  \**
                //  ~~~~~~~~~~~~~~~~~~~  \*
                //  ~~~~~~ filtered ~~~~  q      initial search region
                //  ~~~~~~~~~~~~~~~~~~~  /*      Points in region (*) are needed to eliminate some of the edges in
                //  ~~~~~~~~~~~~~~~~~   /**      the right halfspace.
                //  ~~~~~~~~~~~  -1.25 /***
                search.filter.insert_sector(1.25, -1.25);

                // The search uses a quadtree and only filters bounding boxes that completely lie in the filtered region.
                // Thus points inside the filtered region might be returned. This is necessary to produce dead sectors that
                // overlap with already filtered regions to close of the whole [0, 2pi] range. That in turn permits early
                // abortion of the search.
                size_t iterations{0};
                while(search.next()) {
                    ++iterations;
                    auto neighbor = search.current();

                    // The neighbors are visited in ascending order of distance --> dead sectors are generated in
                    // ascending order of "activation" distance.
                    while (!dead_sectors.empty() && dead_sectors.front().second < neighbor.distance) {
                        search.filter.insert_sector(dead_sectors.front().first);
                        dead_sectors.pop_front();
                    }

                    // Abort if dead sectors cover all directions in the search filter
                    if(search.filter()) {
                        if(std::is_same<Concurrency_tag, Sequential_tag>::value)
                            #ifdef USING_STATS
                            stats.Diamond_test.early_exits++;
                            #endif

                        break;
                    }


                    const FT angle = compute_polar_angle_2(*neighbor.handle);
                    const FT abs_angle = CGAL::abs(angle);

                    CGAL_assertion(0 <= abs_angle && abs_angle <= 2);

                    if(abs_angle < FT{1.45}) {
                        const auto res = set.emplace(neighbor.handle, angle);
                        const auto it = res.first;

                        const auto& q = *query;
                        const auto& nh = *neighbor.handle;

                        if(res.second && less_xy_2(q, nh) && !search.filter(angle)) {

                            bool point_in_left_triangle = false;
                            bool point_in_right_triangle = false;

                            for(auto l = std::next(it); l != set.end() && !point_in_left_triangle; ++l) {
                                // A difference of pi/4.6 radians can result in a difference of up to ~0.525 in pseudo
                                // angles
                                if(l->polar_angle - angle >= 0.525) {
                                    break;
                                }
                                point_in_left_triangle = dt(q, nh, *l->point);
                            }

                            if(point_in_left_triangle)
                            for(auto r = typename Set::reverse_iterator{it}; r != set.rend() && !point_in_right_triangle; ++r) {
                                if(angle - r->polar_angle >= 0.525) {
                                    break;
                                }
                                point_in_right_triangle = dt(nh, q, *r->point);
                            }

                            if (!(point_in_left_triangle && point_in_right_triangle)) {
                                out.emplace_back(index_of(query), index_of(neighbor.handle));
                            }

                        }


                        // Generate dead sector with adjacent neighbors.
                        if (res.second && set.size() > 1) {

                            Construct_dead_sector_2 ds{};

                            // Check for left neighbor
                            if( it != std::prev(set.end()) ) {
                                auto l = std::next(it);
                                if (!search.filter(angle, l->polar_angle)) {

                                    if (auto s = ds(*query, *l->point, *neighbor.handle)) {

                                        CGAL_assertion(s->first <= s->second);
                                        CGAL_assertion(s->first >= angle);
                                        CGAL_assertion(s->second <= l->polar_angle);

                                        dead_sectors.emplace_back(*s, ds.distance_coefficient() * neighbor.distance);
                                    }

                                }
                            }

                            // Check for right neighbor
                            if( it != set.begin()) {
                                auto r = std::prev(it);
                                if (!search.filter(r->polar_angle, angle)) {

                                    if (auto s = ds(*query, *neighbor.handle, *r->point)) {

                                        CGAL_assertion(s->first <= s->second);
                                        CGAL_assertion(s->first >= r->polar_angle);
                                        CGAL_assertion(s->second <= angle);

                                        dead_sectors.emplace_back(*s, ds.distance_coefficient() * neighbor.distance);
                                    }

                                }
                            }

                        }
                    }

                }

                #ifdef USING_STATS
                if(std::is_same<Concurrency_tag, Sequential_tag>::value)
                    stats.Diamond_test.iterations(iterations);
                #endif
            }
        }



        //--------------------------------------------------------------------------------------------------------------
        // Functions related to the LMT-Skeleton.
        //--------------------------------------------------------------------------------------------------------------
        void init_lmt_data_structure() {
            skeleton.clear();
            candidates.clear();
            edge_offsets.clear();

            // Create two halfedges for each edge and store them in compressed sparse row graph format (similar to Boost)
            edge_offsets.resize(points.size() + 1);
            lmt_edges.resize(2 * diamond_edges.size());

            std::vector<uint> counts(points.size());

            edge_offsets.back() = 2 * diamond_edges.size();

            for (const auto &e : diamond_edges) {
                edge_offsets[e.first]++;
                edge_offsets[e.second]++;
            }

            for (size_t i = points.size(); i > 0; --i) {
                edge_offsets[i - 1] = edge_offsets[i] - edge_offsets[i - 1];
            }
            CGAL_assertion(edge_offsets[0] == 0);

            for (const auto &e : diamond_edges) {

                auto i = edge_offsets[e.first] + counts[e.first]++;
                auto j = edge_offsets[e.second] + counts[e.second]++;


                lmt_edges[i] = Halfedge{&points[e.second]};
                lmt_edges[j] = Halfedge{&points[e.first]};

                lmt_edges[i].set_twin(&lmt_edges[j]);
                lmt_edges[j].set_twin(&lmt_edges[i]);
            }

            // Sort edges by polar angle around their source. Update the twin pointers. Init next and start pointers.
            // Update of twin pointers prevents easy parallelization of the for-loop
            for (size_t i = 0; i < points.size(); ++i) {
                auto origin = points[i];

                auto begin = lmt_edges.begin() + edge_offsets[i];
                auto end = lmt_edges.begin() + edge_offsets[i + 1];


                // First partitioning by quadrant followed by a left turn sort is a lot faster than
                // the functor that does both comparisons in one.
                auto m2 = std::partition(begin, end,
                                         [&origin](const Halfedge &e) -> bool { return origin.y() < e.target().y(); });
                auto m1 = std::partition(begin, m2,
                                         [&origin](const Halfedge &e) -> bool { return origin.x() < e.target().x(); });
                auto m3 = std::partition(m2, end,
                                         [&origin](const Halfedge &e) -> bool { return origin.x() > e.target().x(); });

                std::sort(begin, m1, [&origin](const Halfedge &e1, const Halfedge &e2) {
                    return Left_turn_2{}(origin, e1.target(), e2.target());
                });
                std::sort(m1, m2, [&origin](const Halfedge &e1, const Halfedge &e2) {
                    return Left_turn_2{}(origin, e1.target(), e2.target());
                });
                std::sort(m2, m3, [&origin](const Halfedge &e1, const Halfedge &e2) {
                    return Left_turn_2{}(origin, e1.target(), e2.target());
                });
                std::sort(m3, end, [&origin](const Halfedge &e1, const Halfedge &e2) {
                    return Left_turn_2{}(origin, e1.target(), e2.target());
                });



                // Set up next pointers and fix twin pointers
                for (auto it = begin; it != end; ++it) {
                    it->twin()->set_twin(&*it);
                    it->next = &*it + 1;
                }

                if (begin != end) {
                    std::prev(end)->next = &*begin;
                }


                // Init start pointers
                auto e = &*begin;
                for (auto edge = begin; edge != end; ++edge) {

                    while (!edge->has_on_right_side(e->next->target())) {
                        e = e->next;

                        if (e->next == &*edge) {
                            // Convex hull edge
                            ++convex_hull_size;
                            edge->status = edge->twin()->status = Lmt_status::CH;
                            edge->on_stack = edge->twin()->on_stack = false;
                            break;
                        }
                    }

                    if (e->next != &*edge)
                        edge->twin()->j_start = e;
                }


            }
        }


        void lazy_delete(Halfedge_handle e) {
            auto s = index_of(e->source_handle());
            auto t = index_of(e->target_handle());

            e->unlink(&lmt_edges[edge_offsets[s]], &lmt_edges[edge_offsets[s+1]]);
            e->twin()->unlink(&lmt_edges[edge_offsets[t]], &lmt_edges[edge_offsets[t+1]]);
        }


        void lmt_loop(std::vector<Halfedge_handle> &stack) {
            // Main LMT-Loop: Try to find certificates for all edges.
            while (!stack.empty()) {

                auto e = stack.back();
                stack.pop_back();
                e->on_stack = e->twin()->on_stack = false;

                if (!e->find_certificate()) {
                    e->restack_edges(stack);
                    e->status = e->twin()->status = Lmt_status::Impossible;

                    lazy_delete(e);
                }
            }
        }


        void advanced_lmt_loop(std::vector<Halfedge_handle> &stack) {

            while (!stack.empty()) {

                auto e = stack.back();
                stack.pop_back();
                e->on_stack = e->twin()->on_stack = false;

                if (!e->find_local_minimal_certificate()) {
                    e->restack_neighborhood(stack);
                    e->status = e->twin()->status = Lmt_status::Impossible;

                    lazy_delete(e);
                }
            }
        }


        void lmt_loop(Sequential_tag) {
            std::vector<Halfedge_handle> stack;
            stack.reserve(32);

            for (auto& e : lmt_edges) {
                if (e.is_primary() && e.status != Lmt_status::CH) {
                    stack.push_back(&e);
                    lmt_loop(stack);
                }
            }
        }


        #ifdef CGAL_LINKED_WITH_TBB
        void lmt_loop(Parallel_tag) {
            // The parallelization is based on data partitioning. We split the points recursively in two disjoint
            // sets and process them in parallel. Each thread can safely process edges that connect vertices in
            // their respective set. Working concurrently on edges between different point sets leads to a data race and
            // has to be avoided. To increase concurrency, the partitioning is done at the leave level and the unsafe
            // edges are passed back to the caller who will process them in serial. It is important that all edges are
            // marked as being processed (i.e. on_stack = true) initially.

            auto mid = points.begin() + (points.end() - points.begin()) / 2;

            std::vector<Halfedge_handle> left;
            std::vector<Halfedge_handle> right;

            tbb::parallel_invoke([&]{ partition_and_loop(points.begin(), mid, left);},
                                 [&]{ partition_and_loop(mid, points.end(), right);});

            lmt_loop(left);
            lmt_loop(right);

        }


        void partition_and_loop(Point_const_iterator begin, Point_const_iterator end,
                                std::vector<Halfedge_handle> &unsafe_edges) {

            std::vector<Halfedge_handle> safe_edges;

            if(std::distance(begin, end) < 1000) {

                Point_const_handle pb = &*begin;
                Point_const_handle pe = &*end;

                Point_index bi = index_of(begin);
                Point_index ei = index_of(end);

                auto edges_begin = lmt_edges.begin() + edge_offsets[bi];
                auto edges_end = lmt_edges.begin() + edge_offsets[ei];

                for (auto e = edges_begin; e != edges_end; ++e) {
                    if(e->is_primary() && e->status != Lmt_status::CH) {

                        if (e->target_handle() < pb || e->target_handle() >= pe) {
                            unsafe_edges.push_back(&*e);
                        } else {
                            safe_edges.push_back(&*e);
                            lmt_loop(safe_edges);
                        }
                    }
                }

            } else {
                auto mid = begin + (end - begin) / 2;

                std::vector<Halfedge_handle> left;
                std::vector<Halfedge_handle> right;

                tbb::parallel_invoke([&]{ partition_and_loop(begin, mid, left);},
                                     [&]{ partition_and_loop(mid, end, right);});

                Point_const_handle pb = &*begin;
                Point_const_handle pe = &*end;


                for (auto e : boost::join(left, right)) {
                    CGAL_assertion(e->is_primary());

                    if (e->target_handle() < pb || e->target_handle() >= pe) {
                        unsafe_edges.push_back(e);
                    } else {
                        safe_edges.push_back(e);
                        lmt_loop(safe_edges);
                    }
                }

            }
        }


        void mark_certain_edges(Parallel_tag) {
            //TODO: Make this multithreaded
            mark_certain_edges(Sequential_tag{});
        }
        #endif // CGAL_LINKED_WITH_TBB

        void mark_certain_edges(Sequential_tag) {
            skeleton.reserve(points.size() * 3);
            candidates.reserve(points.size() * 1.5);
            // Mark all the edges that have a certificate but no crossing edges as certain.
            for (auto &e : lmt_edges) {
                if (!e.is_primary()) {
                    continue;
                }

                if (e.status == Lmt_status::Possible && !e.has_crossing_edges()) {
                    e.status = e.twin()->status = Lmt_status::Certain;
                    skeleton.push_back(&e);
                } else if (e.status == Lmt_status::CH) {
                    skeleton.push_back(&e);
                } else if (e.status == Lmt_status::Possible) {
                    candidates.push_back(&e);
                }
            }
        }


        // Run the LMT-Skeleton heuristic on the point set and the candidate edges
        void construct_lmt_skeleton() {


            #ifdef USING_STATS
            stats.Lmt_skeleton.time.init = measure_time<std::chrono::nanoseconds>([&] {
                init_lmt_data_structure();
            });
            #else
            init_lmt_data_structure();
            #endif



            CGAL_assertion_code(
                    for(auto& e : lmt_edges) {
                        CGAL_assertion(
                                (e.status == Lmt_status::CH && !e.on_stack && !e.twin()->on_stack)
                                ||
                                (e.status == Lmt_status::Possible && e.on_stack && e.twin()->on_stack)
                        );
                    }
            );


            #ifdef USING_STATS
            stats.Lmt_skeleton.time.lmt_loop = measure_time<std::chrono::nanoseconds>([&] {
                lmt_loop(Concurrency_tag{});
            });


            stats.Lmt_skeleton.time.intersection_test = measure_time<std::chrono::nanoseconds>([&] {
                mark_certain_edges(Concurrency_tag{});
            });

            stats.Lmt_skeleton.possible_edges.after_lmt_loop = candidates.size();
            stats.Lmt_skeleton.certain_edges.after_lmt_loop = skeleton.size();

            stats.Lmt_skeleton.time.advanced_lmt_loop = measure_time<std::chrono::nanoseconds>([&] {
                advanced_certificate_check();
            });

            stats.Lmt_skeleton.possible_edges.after_advanced_lmt_loop = candidates.size();
            stats.Lmt_skeleton.certain_edges.after_advanced_lmt_loop = skeleton.size();
            #else
            lmt_loop(Concurrency_tag{});
            mark_certain_edges(Concurrency_tag{});
            advanced_certificate_check();
            #endif

        }


        // Checks if the certificates (quads) are local minimal,
        // i.e the four quad edges have a certificate that shares a triangle with the quad.
        void advanced_certificate_check() {
            std::vector<Halfedge_handle> stack;
            stack.reserve(candidates.size());

            for(auto e : candidates) {
                stack.push_back(e);
                e->on_stack = e->twin()->on_stack = true;
            }

            advanced_lmt_loop(stack);

            for (auto e : candidates) {
                if (e->status == Lmt_status::Certain) {
                    skeleton.push_back(e);
                } else if (e->status == Lmt_status::Possible && !e->has_crossing_edges()) {
                    e->status = e->twin()->status = Lmt_status::Certain;
                    skeleton.push_back(e);
                }
            }

            candidates.erase(
                    std::remove_if(candidates.begin(), candidates.end(),
                                   [](Halfedge_handle e) { return e->status != Lmt_status::Possible; }),
                    candidates.end()
            );

        }



        //--------------------------------------------------------------------------------------------------------------
        // Functions related to the dynamic programming step.
        //--------------------------------------------------------------------------------------------------------------

        struct Edge_data {

            Edge_data() : src{0}, tar{0}, min_i{nullptr}, min_j{nullptr}, min_weight{0} {}
            Edge_data(uint s, uint t) : src{s}, tar{t}, min_i{nullptr}, min_j{nullptr}, min_weight{0} {}

            uint src;
            uint tar;
            Halfedge_handle min_i;
            Halfedge_handle min_j;
            FT min_weight;
        };

        using Edge_map = CGAL::Unique_hash_map<const Halfedge_handle, Edge_data>;

        struct Face_collector {
            using Iterator = typename std::vector<Halfedge_handle>::const_iterator;
            Iterator begin;
            Iterator end;

            Face_collector(Iterator begin, Iterator end) : begin{begin}, end{end} {}

            boost::optional<Face> operator()() {
                Face face;
                face.reserve();

                for(; begin != end; ++begin) {
                    auto e = *begin;

                    if (!e->visited) {

                        collect_face(e, face);

                        if (face.has_inner_edges()) {
                            return face;
                        }

                        face.clear();
                    }
                }

                return boost::none;
            }

            void collect_face(Halfedge_handle e, Face& face) {

                traverse_boundary_and_collect_edges(e, face.boundary, face.inner_edges);
                CGAL_assertion(!face.boundary.empty());


                if (!face.inner_edges.empty()) {

                    CGAL::Orientation orientation = CGAL::ZERO;
                    CGAL_assertion(CGAL::COUNTERCLOCKWISE != CGAL::ZERO);

                    // Check for faces with holes. Caution: In case of holes new inner edges get pushed back.
                    // Loop only works with indices.
                    for (size_t i{0}; i < face.inner_edges.size(); ++i) {
                        auto inner = face.inner_edges[i];

                        if (!inner->twin()->visited) {
                            // Face has holes and we might have traversed one of the hole boundaries

                            // Check orientation of boundary
                            if (orientation == CGAL::ZERO) {
                                orientation = boundary_orientation(face.boundary);
                            }

                            if (orientation == CGAL::COUNTERCLOCKWISE) {
                                // We traversed the outer face boundary.
                                // The unvisited twin edge belongs to a hole or to an isolated vertex.

                                auto old_size = face.hole_boundaries.size();

                                traverse_boundary_and_collect_edges(inner->twin(), face.hole_boundaries, face.inner_edges);

                                auto new_size = face.hole_boundaries.size();


                                if (new_size - old_size == 0) {
                                    face.isolated_vertices.push_back(inner->target_handle());
                                }

                            } else {
                                // We actually traversed and stored a hole in the face_boundary
                                // The unvisited twin edge might belong to either the outer boundary of the containing face,
                                // another hole or to an isolated vertex.
                                // In one of the iterations we have to hit the face boundary.

                                face.hole_boundaries.insert(face.hole_boundaries.end(),
                                                            face.boundary.begin(), face.boundary.end());
                                face.boundary.clear();
                                orientation = CGAL::ZERO;

                                traverse_boundary_and_collect_edges(inner->twin(), face.boundary, face.inner_edges);

                                if (face.boundary.empty()) {
                                    face.isolated_vertices.push_back(inner->target_handle());
                                }
                            }

                        }

                    }
                }

            }


            CGAL::Orientation boundary_orientation(const typename Face::Edge_container_type &face_boundary) const {
                if (face_boundary.empty()) return CGAL::CLOCKWISE;

                auto min = std::min_element(face_boundary.begin(), face_boundary.end(),
                                            [](const Halfedge_handle lhs, const Halfedge_handle rhs) {
                                                return Less_xy_2{}(lhs->target(), rhs->target());
                                            });

                auto next = std::next(min) == face_boundary.end() ? face_boundary.begin() : std::next(min);

                if ((*min)->has_on_left_side((*next)->target())) {
                    return CGAL::Orientation::COUNTERCLOCKWISE;
                } else {
                    return CGAL::Orientation::CLOCKWISE;
                }
            }


            void traverse_boundary_and_collect_edges(Halfedge_handle e,
                                                     typename Face::Edge_container_type &boundary_edges,
                                                     typename Face::Edge_container_type &inner_edges) {

                // Traversal of the boundary that contains the source vertex of e.
                // Might be:
                // (i) CW (reverse) iteration of the outer boundary a face
                // (ii) CCW (reverse) iteration of the inner boundary a hole
                // (iii) The source is an isolated vertex and we just loop through the possible edges around the source vertex
                //       without ever hitting any boundary edge at all.

                auto old_end = std::distance(boundary_edges.begin(), boundary_edges.end());

                e->visited = true;
                if (e->status == Lmt_status::Possible) {
                    inner_edges.push_back(e);
                }

                auto *next = e;
                do {
                    next = next->next_edge();

                    while (next->status == Lmt_status::Possible && next != e) {
                        inner_edges.push_back(next);
                        next->visited = true;
                        next = next->next_edge();
                    }

                    if (next->status != Lmt_status::Possible) {
                        CGAL_assertion(next->status == Lmt_status::Certain || next->status == Lmt_status::CH);
                        next = next->twin();
                        next->visited = true;
                        boundary_edges.push_back(next);
                    }

                } while (next != e);

                // Change new boundary edges to standard orientation
                std::reverse(std::next(boundary_edges.begin(), old_end), boundary_edges.end());
            }
        };

        void collect_and_triangulate_faces() {

            for (auto e : skeleton) {
                (void)e; // supress unused-parameter warning
                CGAL_assertion(!e->visited);
                CGAL_assertion(e->status == Lmt_status::Certain || e->status == Lmt_status::CH);
            }

            collect_and_triangulate_faces(Concurrency_tag{});
        }


        void collect_and_triangulate_faces(Sequential_tag) {

            Face_collector collector{skeleton.begin(), skeleton.end()};

            while(auto face = collector()) {
                #ifdef USING_STATS
                stats.Triangulation.faces.total++;
                if (face->is_simple())
                    stats.Triangulation.faces.simple++;
                #endif

                triangulate_face(std::move(*face));
            }
        }

        #ifdef CGAL_LINKED_WITH_TBB
        void collect_and_triangulate_faces(Parallel_tag) {

            Face_collector collector{skeleton.begin(), skeleton.end()};

            auto serial_producer =  tbb::make_filter<void, Face>(
                    tbb::filter::serial,
                    [&](tbb::flow_control& fc) -> Face {

                        if(auto face = collector()) {
                            #ifdef USING_STATS
                            stats.Triangulation.faces.total++;
                            if (face->is_simple())
                                stats.Triangulation.faces.simple++;
                            #endif

                            return std::move(*face);
                        }
                        else {
                            fc.stop();
                            return Face{};
                        }

                    }
            );

            auto parallel_consumer = tbb::make_filter<Face, void>(
                    tbb::filter::parallel,
                    [&](Face face) {
                        triangulate_face(std::move(face));
                    }
            );


            tbb::parallel_pipeline(2 * std::thread::hardware_concurrency(),
                serial_producer
                &
                parallel_consumer
            );
        }
        #endif // CGAL_LINKED_WITH_TBB

        std::function<void(Face&)> simple_face_callback;
        std::function<void(Face&)> face_with_holes_callback;

        std::function<void(IloEnv*, IloModel*, IloCplex*, IloBoolVarArray*, unsigned int, unsigned int)> pre_ip_solve_callback;
        std::function<void(IloEnv*, IloModel*, IloCplex*, IloBoolVarArray*, unsigned int)> post_ip_solve_callback;

        void triangulate_face(Face&& face) {
            if (face.hole_boundaries.empty() && face.isolated_vertices.empty()) {
                simple_faces_count++;

                if(simple_face_callback) simple_face_callback(face);
                if(triangulate_simple_faces) triangulate_simple_face(std::move(face.boundary));
            } else {
                faces_w_holes_count++;
                if(face_with_holes_callback) face_with_holes_callback(face);
                if(triangulate_faces_with_holes) triangulate_face_with_holes(std::move(face));
            }
        }

        void triangulate_simple_face(typename Face::Edge_container_type &&face) {
            if (face.size() <= 3)
                return;

            Edge_map map{Edge_data{}, 64};

            std::vector<Halfedge_handle> inner_edges;
            inner_edges.reserve(64);

            // Init indices for all edge endpoints.
            // Edges sharing an endpoint can have different indices for that point in degenerate cases, e.g.
            // the polygon contains an antenna and the same vertex appears twice on the boundary.
            uint index = 0;
            for (auto e : face) {
                map[e] = Edge_data{index, index + 1};
                if(!map.is_defined(e->twin()))
                    map[e->twin()] = Edge_data{index + 1, index};

                auto next = e->next_edge();

                while (next->status == Lmt_status::Possible) {
                    inner_edges.push_back(next);
                    map[next].src = index;
                    map[next->twin()].tar = index;

                    next = next->next_edge();
                }
                ++index;
            }
            map[face.back()].tar = 0;
            map[face.back()->twin()].src = 0;

            CGAL_assertion(face.size() == index);

            // Sort inner edges by the number of boundary vertices between them
            // Necessary to compute the overall solution bottom up.
            std::sort(inner_edges.begin(), inner_edges.end(), [&](const Halfedge_handle lhs, const Halfedge_handle rhs) {
                CGAL_assertion(map.is_defined(lhs) && map.is_defined(rhs));
                const auto &l = map[lhs];
                const auto &r = map[rhs];
                auto dl = (l.src - l.tar + index) % index;
                auto dr = (r.src - r.tar + index) % index;

                return dl < dr;
            });

            // After all inner edges are processed, the overall solution can be obtained with any boundary edge.
            inner_edges.push_back(face.front());

            // Compute solution
            for (auto e : inner_edges) {
                auto &e_data = map[e];

                // Triangles with two boundary edges. Boundary edges have weight 0.
                if (((e_data.src - e_data.tar + index) % index) == 2) {
                    e_data.min_weight = e->length();
                    continue;
                }

                e_data.min_weight = std::numeric_limits<FT>::infinity();
                e->reset();
                while (e->next_triangle()) {

                    // Check that the next_triangle function really yielded an empty triangle.
                    if (!map.is_defined(e->i) || !map.is_defined(e->j)) {
                        continue; // Triangle crosses boundary edge and is part of two adjacent polygons
                    }

                    if ((e->j->status == Lmt_status::Possible && map[e->j].src != e_data.tar)
                        ||
                        (e->i->status == Lmt_status::Possible && map[e->i].tar != map[e->j].tar))
                    {
                       continue; // Degenerate cases caused by polygons with antennas
                    }


                    FT weight{0};

                    if (e->i->status == Lmt_status::Possible)
                        weight += map[e->i].min_weight;

                    if (e->j->status == Lmt_status::Possible)
                        weight += map[e->j->twin()].min_weight;


                    if (weight < e_data.min_weight) {
                        e_data.min_weight = weight;
                        e_data.min_i = e->i;
                        e_data.min_j = e->j->twin();
                    }
                }

                e_data.min_weight += e->length();
            }

            inner_edges.clear();

            // Extract solution
            CGAL_assertion(map.is_defined(face.front()));
            const auto &f_data = map[face.front()];

            if (f_data.min_i->status == Lmt_status::Possible)
                inner_edges.push_back(f_data.min_i);

            if (f_data.min_j->status == Lmt_status::Possible)
                inner_edges.push_back(f_data.min_j);

            while (!inner_edges.empty()) {
                auto e = inner_edges.back();
                inner_edges.pop_back();

                e->status = e->twin()->status = Lmt_status::Certain;
                face_triangulation_edges.push_back(e->primary_edge());

                auto e_data = map[e];

                if (e_data.min_i != nullptr && e_data.min_i->status == Lmt_status::Possible) {
                    inner_edges.push_back(e_data.min_i);
                }
                if (e_data.min_j != nullptr && e_data.min_j->status == Lmt_status::Possible) {
                    inner_edges.push_back(e_data.min_j);
                }
            }


        }

        void triangulate_face_with_holes(Face &&face) {

            if(cplex_logging)
                std::cout << "Found complex face with "
                          << face.boundary.size() << " boundary edges, "
                          << face.hole_boundaries.size() << " hole edges, "
                          << face.isolated_vertices.size() << " isolated vertices."
                          << std::endl;

            auto start = std::chrono::high_resolution_clock::now();

            std::set<Halfedge_handle> all_edges;

            auto insert_edges = [&all_edges](std::vector<Halfedge_handle> &edges_to_insert)
            {
                for (auto e : edges_to_insert) {
                    all_edges.insert(e->primary_edge());
                }
            };

            insert_edges(face.inner_edges);
            insert_edges(face.boundary);
            insert_edges(face.hole_boundaries);

            CGAL_assertion_code(
                for (auto e : face.inner_edges) {
                    CGAL_assertion(e->status == CGAL::Lmt_status::Possible && e->twin()->status == CGAL::Lmt_status::Possible);
                }
            );

            CGAL_assertion_code(
                for (auto e : face.boundary) {
                    CGAL_assertion((e->status == CGAL::Lmt_status::Certain || e->status == CGAL::Lmt_status::CH) ||
                                (e->twin()->status == CGAL::Lmt_status::Certain || e->twin()->status == CGAL::Lmt_status::CH));
                }
            );

             CGAL_assertion_code(
                for (auto e : face.hole_boundaries) {
                    CGAL_assertion(e->status == CGAL::Lmt_status::Certain || e->twin()->status == CGAL::Lmt_status::Certain);
                }
            );

            if (cplex_logging)
            {
                std::cout << "Start finding all possible triangles in face." << std::endl;
            }
            std::vector<MWT::EmptyTriangle<Traits>> triangles =
                    utils::ip_solver::get_empty_triangles<Traits, Halfedge_handle, Face>(face, all_edges, &ip_solver_mutex, cplex_logging);

            // solve IP
            minimum_weight_triangulation_ip_solver<Traits> solver(triangles, all_edges, cplex_logging,
                    std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now() - start).count());
            solver.register_pre_ip_solve_callback(pre_ip_solve_callback);
            solver.register_post_ip_solve_callback(post_ip_solve_callback);

            if(cplex_logging && !solver.is_lp_relaxation_integral())
            {
                std::cout << "The LP was not integral!" << std::endl;
            }

            auto res = solver.solve();

            CGAL_assertion(res.solved);

            for (auto &t : res.triangles) {
                set_inner_face_halfedge_certain(t.e1);
                set_inner_face_halfedge_certain(t.e2);
                set_inner_face_halfedge_certain(t.e3);

                CGAL_assertion(t.e1 != t.e2 && t.e2 != t.e3 && t.e1 != t.e3);
            }
        }

        void set_inner_face_halfedge_certain(Halfedge_handle e) {
            if (e->status == CGAL::Lmt_status::Possible) {
                face_triangulation_edges.push_back(e->primary_edge());
                e->status  = CGAL::Lmt_status::Certain;
                e->twin()->status =  CGAL::Lmt_status::Certain;
            }
        }

    };
}


#endif //MWT_MINIMUM_WEIGHT_TRIANGULATION_H
