#ifndef MWT_LMT_HALFEDGE_H
#define MWT_LMT_HALFEDGE_H


#include <CGAL/assertions.h>
#include <CGAL/enum.h>
#include <CGAL/Algebraic_structure_traits.h>

namespace CGAL {

    enum class Lmt_status : char {
        Possible,
        Certain,
        Impossible,
        CH
    };


    //------------------------------------------------------------------------------------------------------------------
    // Halfedge struct.
    //------------------------------------------------------------------------------------------------------------------
    template<typename Traits>
    struct Lmt_halfedge {

        using FT = typename Traits::FT;
        using Point_2 = typename Traits::Point_2;

        using Left_turn_2 = typename Traits::Left_turn_2;
        using Compute_squared_distance_2 = typename Traits::Compute_squared_distance_2;
        using Less_distance_2 = typename Traits::Less_distance_2;



        Lmt_halfedge() = default;

        Lmt_halfedge(const Point_2 *target) :
                tar{target}, twn{nullptr}, next{nullptr}, i{nullptr}, j{nullptr}, j_start{nullptr},
                status{Lmt_status::Possible}, on_stack{true} {};


        // Check whether quadrilateral abcd with diagonals ac and bd is convex.
        static bool is_convex(const Point_2 &a, const Point_2 &b, const Point_2 &c, const Point_2 &d) {
            CGAL_precondition(Left_turn_2{}(a, c, d) && Left_turn_2{}(c, a, b));
            return Left_turn_2{}(b, d, a) && Left_turn_2{}(d, b, c);
        }

        // Find the next (most likely empty) triangle on the left side of this halfedge.
        // In a full edge set the triangle would be guaranteed to be empty. However, the candidate edges are prefiltered
        // by the diamond test and there is a chance (due to a missing edge to a nearby point) that we find a non-empty
        // triangle. The overall algorithm is still correct in the sense that it produces a subset of the MWT.
        // Additionally, it seems that all of the non-empty triangles get eliminated anyway by the end of the algorithm.
        bool next_triangle() {
            CGAL_precondition(status != Lmt_status::Impossible);

            if (j == nullptr) {
                // We already iterated through all triangles in an earlier call.
                return false;
            }

            // Break the current triangle.
            // (On the first call after a reset it might actually create the very first valid triangle)
            j = j->next_edge();

            while (j->target_handle() != source_handle()) {

                // Rotate the left edge until we hit the same target (triangle found) or pass beyond it.
                while (i->target_handle() != j->target_handle() && !j->has_on_left_side(i->target())) {
                    CGAL_assertion(has_on_left_side(i->target()));
                    i = i->next_edge();

                }

                // Now do the same with the right edge.
                // If we hit the source vertex, we went so far that there are no more triangles.
                while (j->target_handle() != i->target_handle() && j->target_handle() != source_handle()
                       && j->has_on_left_side(i->target())) {
                    j = j->next_edge();
                }


                // Repeat the above process until a triangle is found or one of edges went too far
                if (i->target_handle() == j->target_handle()) {
                    CGAL_assertion(has_on_left_side(i->target()));
                    return true;
                } else if (!has_on_left_side(i->target())) { // Left edge went too far
                    j = nullptr; //Just to make sure later calls will exit early
                    return false;
                }

            }

            // Right edge went too far
            j = nullptr;
            return false;
        }


        void reset() {
            CGAL_precondition(j_start != nullptr);
            i = next_edge();
            j = j_start;
        }


        // Tries to find the next valid certificate for this edge.
        // A certificate is a pair of triangles that form a quadrilateral q such that q is either non-convex or this
        // edge is the shorter diagonal in q.
        bool find_certificate() {
            CGAL_precondition(is_primary());
            CGAL_precondition(status != Lmt_status::Impossible);

            if (!has_certificate()) { // Called for the first time for this edge
                reset();
                twin()->reset();
                if (!twin()->next_triangle())
                    return false;
            }

            // After restacking this edge, at least one of the triangles is invalid.
            // Special treatment is needed if the triangle on the twin side is invalid.
            if (!twin()->has_valid_triangle()) {
                if (twin()->next_triangle())
                    reset();
                else
                    return false;
            }

            // Try all quadrilaterals, i.e. all pairs of triangles on opposite sides of this edge.
            while (true) {

                if (!next_triangle()) {
                    if (twin()->next_triangle())
                        reset();
                    else
                        return false;
                } else {
                    CGAL_assertion(has_valid_triangle() && twin()->has_valid_triangle());
                    // A quadrilateral is a certificate iff this edge is the shorter diagonal or it is not convex.
                    if (!Less_distance_2{}(twin()->i->target(), i->target(), source(), target()) ||
                        !is_convex(source(), twin()->i->target(), target(), i->target())) {
                        return true;
                    }
                }
            }
        }


        // TODO: Can probably be improved. Checks the same triangles multiple times for local minimality.
        bool find_local_minimal_certificate() {
            CGAL_precondition(is_primary());
            CGAL_precondition(status != Lmt_status::Impossible);
            while (!has_valid_triangle() || !twin()->has_valid_triangle() ||
                   !has_local_minimal_triangle() || !twin()->has_local_minimal_triangle()) {
                if (!find_certificate()) {
                    return false;
                }
            }

            return true;

        }

        // Restack those edges whose local minimal certificate might get invalidated when this edge becomes impossible.
        // Kinda difficult to figure out the exact set of edges that need to be restacked. It is a subset of the
        // neighborhood, i.e. the outgoing edges and their outgoing edges. We restack them all.
        void restack_neighborhood(std::vector<Lmt_halfedge *> &stack) const {
            CGAL_precondition(status != Lmt_status::Impossible);

            // Neighborhood of source vertex
            auto e = next_edge();
            while (e != this) {

                auto ne = e->twin();
                do {

                    if (!ne->on_stack && ne->status == Lmt_status::Possible) {
                        stack.push_back(ne->primary_edge());
                        ne->on_stack = ne->twn->on_stack = true;
                    }
                    ne = ne->next_edge();

                } while (ne != e->twin());

                e = e->next_edge();
            }

            // Neighborhood of target vertex
            e = twn->next_edge();
            while (e != twn) {

                auto ne = e->twin();
                do {

                    if (!ne->on_stack && ne->status == Lmt_status::Possible) {
                        stack.push_back(ne->primary_edge());
                        ne->on_stack = ne->twn->on_stack = true;
                    }
                    ne = ne->next_edge();

                } while (ne != e->twin());

                e = e->next_edge();
            }
        }


        // A triangle t is local minimal iff all three edges e of t meet either condition:
        // (i) e is on the convex hull
        // (ii) e is local minimal w.r.t a certificate that has t in common
        bool has_local_minimal_triangle() {
            CGAL_precondition(status != Lmt_status::Impossible); // This edge is local minimal
            CGAL_precondition(has_valid_triangle());

            // Make copies of i and j and try to find a certificate for them
            bool left_valid{false};
            bool right_valid{false};

            if (i->status == Lmt_status::CH) {
                left_valid = true;
            } else {
                Lmt_halfedge left = *i;
                left.reset();

                while (!left_valid && left.next_triangle()) {
                    left_valid |= !Less_distance_2{}(target(), left.i->target(), left.source(), left.target()) ||
                                  !is_convex(left.source(), target(), left.target(), left.i->target());
                }
            }

            if (j->status == Lmt_status::CH) {
                right_valid = true;
            } else {
                Lmt_halfedge right = *j->twin();
                right.reset();

                while (left_valid && !right_valid && right.next_triangle()) {
                    right_valid |=
                            !Less_distance_2{}(source(), right.i->target(), right.source(), right.target()) ||
                            !is_convex(right.source(), source(), right.target(), right.i->target());
                }
            }

            return left_valid && right_valid;
        }


        // Check all neighbors of the source vertex on the left side of this edge for an outgoing intersecting edge
        bool has_crossing_edges_on_left_side() const {
            auto n = next_edge();
            while (has_on_left_side(n->target())) {
                auto edge = n->twin()->next_edge();

                CGAL_assertion(has_on_left_side(edge->source()));

                while (edge->target_handle() != target_handle() && edge->has_on_left_side(target())) {
                    // This holds because the edges are sorted by angle and the edge set
                    // always contains a triangulation.
                    // There should always be an edge that either connects to the target_handle or has the target on
                    // the right side before there is an edge that wraps around so much that this would fail.
                    CGAL_assertion(edge->has_on_right_side(source()));

                    if (has_on_right_side(edge->target())) {
                        return true;
                    }
                    edge = edge->next_edge();

                    // Same reason as above
                    CGAL_assertion(edge->target_handle() != source_handle());
                }
                n = n->next_edge();
            }

            return false;
        }

        bool has_crossing_edges() const {
            // The candidate edges contain a valid triangulation at any point in time. Therefore, it suffices to
            // check the neighbors for outgoing intersecting edges. Indeed it suffices to check only the neighbors of
            // one side of one vertex.
            return has_crossing_edges_on_left_side();
        }


        // Restacks exactly those edges whose certificates become invalid when this edge becomes impossible.
        void restack_edges(std::vector<Lmt_halfedge *> &stack) const {
            CGAL_precondition(status != Lmt_status::Impossible);

            auto e = next_edge();
            while (e != this) {
                if (!e->on_stack && e->status == Lmt_status::Possible) {
                    if (e->twn->j == this || e->i == this) {
                        stack.push_back(e->primary_edge());
                        e->on_stack = e->twn->on_stack = true;
                    }
                }
                e = e->next_edge();
            }

            e = twn->next_edge();
            while (e != twn) {
                if (!e->on_stack && e->status == Lmt_status::Possible) {
                    if (e->twn->j == twn || e->i == twn) {
                        stack.push_back(e->primary_edge());
                        e->on_stack = e->twn->on_stack = true;
                    }
                }
                e = e->next_edge();
            }

        }


        Lmt_halfedge *next_edge() const {
            CGAL_assertion(next->status != Lmt_status::Impossible);
            return next;
        }

        void unlink(Lmt_halfedge* b, Lmt_halfedge* e) {
            CGAL_assertion(status == Lmt_status::Impossible);

            --e;
            CGAL_assertion(b->source_handle() == source_handle());
            CGAL_assertion(e->source_handle() == source_handle());

            auto prev = this - 1;
            if(prev < b)
                prev = e;

            do {
                prev->next = next;
                --prev;

                if(prev < b)
                    prev = e;
            } while(prev->next == this);
        }

        Lmt_halfedge *primary_edge() {
            return is_primary() ? this : twin();
        }

        // Define one of the halfedges of each pair as the primary one. The choice is more or less arbitrary.
        bool is_primary() const {
            return source_handle() < target_handle();
        }

        const Point_2 &target() const {
            return *tar;
        }

        const Point_2 &source() const {
            return twn->target();
        }

        const Point_2 *target_handle() const {
            return tar;
        }

        const Point_2 *source_handle() const {
            return twn->tar;
        }

        const Lmt_halfedge *twin() const {
            CGAL_assertion(twn != nullptr);
            return twn;
        }

        Lmt_halfedge *twin() {
            CGAL_assertion(twn != nullptr);
            return twn;
        }

        void set_twin(Lmt_halfedge *twin) {
            twn = twin;
        }

        bool has_certificate() const {
            bool b = i != nullptr && j != nullptr;
            CGAL_assertion(!b || (twin()->i != nullptr && twin()->j != nullptr));
            return b;
        }

        bool has_valid_triangle() const {
            return i != nullptr && j != nullptr
                   && i->status != Lmt_status::Impossible
                   && j->status != Lmt_status::Impossible
                   && i->target_handle() == j->target_handle();
        }

        bool has_on_left_side(const Point_2 &p) const {
            return Left_turn_2{}(source(), target(), p);
        }

        bool has_on_right_side(const Point_2 &p) const {
            return Left_turn_2{}(target(), source(), p);
        }

        bool is_certain() const {
            return this->status == CGAL::Lmt_status::Certain;
        }

        FT length() const {
            return typename CGAL::Algebraic_structure_traits<FT>::Sqrt{}(
                    Compute_squared_distance_2{}(source(), target())
            );
        }


        // Target of this halfedge.
        const Point_2 *tar;
        // Twin of this halfedge.
        Lmt_halfedge *twn;
        // The next outgoing halfedge in ccw order.
        Lmt_halfedge *next;
        // The left halfedge of the certificate.
        Lmt_halfedge *i;
        // The right halfedge of the certificate.
        Lmt_halfedge *j;
        // Entry point for the triangle search.
        Lmt_halfedge *j_start;

        Lmt_status status;
        union {
            bool on_stack;
            bool visited;
        };
    };

}


#endif //MWT_LMT_HALFEDGE_H
