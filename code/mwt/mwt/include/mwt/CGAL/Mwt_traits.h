#ifndef MWT_TRAITS_H
#define MWT_TRAITS_H

#include <type_traits>
#include <boost/optional.hpp>

#include <CGAL/enum.h>
#include <CGAL/number_utils.h>
#include <CGAL/Filtered_predicate.h>
#include <CGAL/Exact_rational.h>
#include <CGAL/Cartesian_converter.h>
#include <CGAL/Interval_nt.h>
#include <CGAL/predicates/kernel_ftC2.h>
#include <CGAL/Simple_cartesian.h>

namespace CGAL {

    namespace Minimum_weight_triangulation {

        template<typename Kernel_>
        class Mwt_traits_2 : public Kernel_ {
        public:
            using Kernel = Kernel_;
            using Point_2 = typename Kernel::Point_2;
            using Direction_2 = typename Kernel::Direction_2;

            using FK = Simple_cartesian<Interval_nt_advanced>;
            using EK = Simple_cartesian<Exact_rational>;
            using C2E = Cartesian_converter<Kernel, EK>;
            using C2F = Cartesian_converter<Kernel, FK>;
            using E2C = Cartesian_converter<EK, Kernel>;
            using F2C = Cartesian_converter<FK, Kernel>;


        private:

            struct Angle_less {
                Angle_less(const Point_2& p) : origin{p} {}

                bool operator()(const Point_2& p, const Point_2& q) const {
                    return Direction_2( p - origin) < Direction_2( q - origin);
                }

            private:
                const Point_2 origin;
            };


            //----------------------------------------------------------------------------------------------------------
            // We need to store and compare intervals of polar angles. The actual value is not important as long as the
            // ordering is still correct. The following two functions are much faster than atan2 or dot-product angle
            // calculations

            // Computes polar angle based on the idea of taxicab geometry (L1-norm). Basically radians w.r.t to the L1-
            // circle.
            // It maps [0, 2PI] --> [0, 4]
            template<typename K>
            struct Compute_L1_angle {
                using FT = typename K::FT;

                static constexpr double min_polar_angle = 0.0;
                static constexpr double max_polar_angle = 4.0;

                FT operator()(const FT& x, const FT& y) const {
                    if (y >= 0)
                        return (x >= 0 ? y / (x + y)
                                       : 1 - x / (-x + y));
                    else
                        return (x < 0 ? 2 - y / (-x - y)
                                      : 3 + x / (x - y));
                }
            };


            //  This functions gets compiled to branch-free assembly for floating point types
            //  and is probably the fastest way to do it.
            //  It maps: [0, PI] --> [0, 2] and (PI, 2 PI] --> [-2, 0].
            //  Discontinuity is at PI were it jumps from 2 to -2, similar to atan2
            template<typename K>
            struct Compute_pseudo_angle {
                using FT = typename K::FT;

                static constexpr double min_polar_angle = -2.0;
                static constexpr double max_polar_angle = 2.0;

                template <typename T = FT>
                FT operator()(typename std::enable_if<std::is_floating_point<T>::value, T>::type x,
                              typename std::enable_if<std::is_floating_point<T>::value, T>::type y) const
                {
                    return std::copysign(FT{1} - x / (std::fabs(x) + std::fabs(y)), y);
                }

                template <typename T = FT>
                FT operator()(const typename std::enable_if<!std::is_floating_point<T>::value, T>::type &x,
                              const typename std::enable_if<!std::is_floating_point<T>::value, T>::type &y) const
                {
                    FT r = CGAL::abs( FT{1} - x / (CGAL::abs(x) + CGAL::abs(y)) );
                    return y >= 0 ? r : -r;
                }
            };


            template <typename K, template <typename T> class  A>
            struct Compute_polar_angle {
                using FT = typename K::FT;
                using Point_2 = typename K::Point_2;
                using Angle = A<K>;

                Compute_polar_angle () : origin{} {}

                Compute_polar_angle(const Point_2& p) : origin{p} {}

                FT operator()(const Point_2& p) const {
                    return Angle{}(p.x() - origin.x(),
                                   p.y() - origin.y());
                }

                FT operator()(const FT& x, const FT& y) const {
                    return Angle{}(x - origin.x(),
                                   y - origin.y());
                }

            private:
                const Point_2 origin;
            };


            template <typename K>
            struct Less_polar_angle {
                using FT = typename K::FT;
                using Point_2 = typename K::Point_2;
                using result_type = bool;

                Less_polar_angle(const Point_2& p) : origin{p} {}

                result_type operator() (const Point_2& p, const Point_2& q) const {
                    return compare_angle_with_x_axisC2(p.x() - origin.x(), p.y() - origin.y(),
                                                       q.x() - origin.x(), q.y() - origin.y())
                           == Comparison_result::SMALLER;
                }

            private:
                const Point_2 origin;
            };

            //--------------------------------------------------------------------------------------------------------//


            template <typename K, template <typename T> class A>
            struct Construct_dead_sector {
                using FT = typename K::FT;
                using Point_2 = typename K::Point_2;
                using Sector_2 = std::pair<FT, FT>;
                using result_type = boost::optional<Sector_2>;
                using Angle = A<K>;

                constexpr double cos_alpha () const {
                    return 0.7757113;
                }

                constexpr double sin_alpha () const {
                    return 0.63108794;
                }

                constexpr double squared_cos_alpha () const {
                    return 0.6017281;
                }

                constexpr double squared_cos_2alpha () const {
                    return 0.04139435;
                }

                constexpr double distance_coefficient() const {
                    return 2.407;
                }

                result_type operator()(const Point_2& p, const Point_2& l, const Point_2& r) const {

                    result_type sector{};

                    const FT& px = p.x();
                    const FT& py = p.y();
                    const FT& rx = r.x();
                    const FT& ry = r.y();
                    const FT& lx = l.x();
                    const FT& ly = l.y();


                    // If the angle spanned by vectors pr and pl is larger than alpha = 2*pi/4.6 --> no dead sector

                    const FT vlx = lx - px;
                    const FT vly = ly - py;
                    const FT vrx = rx - px;
                    const FT vry = ry - py;

                    const FT dot = vlx * vrx + vly * vry;
                    const FT dot_squared = dot * dot;

                    if(dot <= FT{0}) {
                        return sector;
                    }

                    CGAL_assertion(CGAL::left_turn(p, r, l));

                    // cos^2( <(l,r) ) * (||l|| * ||r||)^2 = (l * r)^2 = dot^2

                    const FT nl = vlx * vlx + vly * vly;
                    const FT nr = vrx * vrx + vry * vry;
                    const FT denom = (nl * nr);

                    FT lower{0};
                    FT upper{0};

                    if(dot_squared <= squared_cos_2alpha() * denom) { // angle >= 2 * alpha = PI/2.3
                        // No dead sector
                        return sector;
                    } else if(dot_squared >= squared_cos_alpha() * denom) { // angle <= alpha = PI/4.6
                        // The dead sector is bounded by the two vertices
                        Angle angle{};
                        lower = angle(vrx, vry);
                        upper = angle(vlx, vly);
                    } else { // alpha < angle < 2 * alpha

                        // Rotate right vector to the left (ccw)
                        const FT slx = cos_alpha() * vrx - sin_alpha() * vry;
                        const FT sly = sin_alpha() * vrx + cos_alpha() * vry;

                        // Rotate left vector to the right (cw)
                        const FT srx = cos_alpha() * vlx + sin_alpha() * vly;
                        const FT sry = - sin_alpha() * vlx + cos_alpha() * vly;

                        // The dead sector is the overlapping between the rotated vectors.
                        Angle angle{};
                        lower = angle(srx, sry);
                        upper = angle(slx, sly);
                    }

                    // Numerical errors may generate degenerate sectors (lower >= upper)
                    // Returning a smaller or an empty sector is always fine.
                    if(lower < upper) {
                        sector.emplace(lower, upper);
                    }
                    return sector;

                }
            };


            template <typename K>
            struct Diamond_test {
                using FT = typename K::FT;
                using Point_2 = typename K::Point_2;
                using Segment_2 = typename K::Segment_2;
                using result_type = bool;

                //Assumption l lies on the left side of the edge, r on the right side
                result_type operator()(const Segment_2& e, const Point_2& l, const Point_2& r) const {
                    return both_triangles_occupied(e.source(), e.target(), l, r);
                }

                result_type operator()(const Point_2& source,const Point_2& target, const Point_2& left, const Point_2& right) const {
                    return both_triangles_occupied(source, target, left, right);
                }

                //Assumption l lies on the left side of the edge
                result_type operator()(const Segment_2& e, const Point_2& l) const {
                    return left_triangle_occupied(e.source(), e.target(), l);
                }

                //Assumption l lies on the left side of the edge
                result_type operator()(const Point_2& source, const Point_2& target, const Point_2& left) const {
                    return left_triangle_occupied(source, target, left);
                }


            private:

                /* Bound for Diamond Property: PI / 4.6
                *  Proved by Drysdale, McElfresh and Snoeyink in
                *  "An improved diamond property for minimum-weight triangulation" (1998)
                *
                *  An edge e can only be part of the MWT if at least one of the adjacent isosceles triangles with angle
                *  alpha = PI / 4.6 is empty, i.e. it contains no other points of the point set in question.
                */
                constexpr double half_tan_alpha() const { return 0.4067801718813224277; }


                result_type left_triangle_occupied(const Point_2 &s, const Point_2 &t,
                                                   const Point_2 &l) const {

                    const FT& sx = s.x();
                    const FT& sy = s.y();
                    const FT& tx = t.x();
                    const FT& ty = t.y();

                    // Midpoint of edge st
                    const FT mx = FT{0.5} * (sx + tx);
                    const FT my = FT{0.5} * (sy + ty);

                    // Orthogonal vector to st with equal length
                    const FT ox = (ty - sy);
                    const FT oy = (sx - tx);

                    // Apex of left triangle
                    const FT alx = mx - half_tan_alpha() * ox;
                    const FT aly = my - half_tan_alpha() * oy;

                    // Points must be in CCW order
                    return  in_triangle(sx, sy, tx, ty, alx, aly, l.x(), l.y());
                }

                result_type both_triangles_occupied(const Point_2 &s, const Point_2 &t,
                                                    const Point_2 &l, const Point_2 &r) const
                {

                    const FT& sx = s.x();
                    const FT& sy = s.y();
                    const FT& tx = t.x();
                    const FT& ty = t.y();

                    // Midpoint of edge st
                    const FT mx = FT{0.5} * (sx + tx);
                    const FT my = FT{0.5} * (sy + ty);

                    // Orthogonal vector to st with equal length
                    const FT ox = (ty - sy);
                    const FT oy = (sx - tx);

                    // Apex of right triangle
                    const FT arx = mx + half_tan_alpha() * ox;
                    const FT ary = my + half_tan_alpha() * oy;

                    // Apex of left triangle
                    const FT alx = mx - half_tan_alpha() * ox;
                    const FT aly = my - half_tan_alpha() * oy;

                    // Points must be in CCW order
                    return  in_triangle(sx, sy, tx, ty, alx, aly, l.x(), l.y()) &&
                            in_triangle(sx, sy, arx, ary, tx, ty, r.x(), r.y());

                }

                // Assumption: Triangle abc is given in CCW order
                result_type in_triangle(const FT& ax, const FT& ay,
                                        const FT& bx, const FT& by,
                                        const FT& cx, const FT& cy,
                                        const FT& px, const FT& py) const
                {
                    return orientationC2(px, py, ax, ay, bx, by) == Orientation::COUNTERCLOCKWISE &&
                           orientationC2(px, py, bx, by, cx, cy) == Orientation::COUNTERCLOCKWISE &&
                           orientationC2(px, py, cx, cy, ax, ay) == Orientation::COUNTERCLOCKWISE;
                }

            };



            template <typename K>
            struct Compare_squared_radius {
                using FT = typename K::FT;
                using Point_2 = typename K::Point_2;
                using result_type = Comparison_result;

                result_type operator()(const Point_2& p1, const Point_2& p2, const Point_2& p3, const FT& sr) const {
                    K kernel{};
                    auto r = kernel.compute_squared_radius_2_object()(p1, p2, p3);

                    return compare(r, sr);
                }
            };

            template <typename K>
            struct Less_distance {
                using FT = typename K::FT;
                using Point_2 = typename K::Point_2;
                using result_type = bool;

                result_type operator()(const Point_2& p, const Point_2& q, const Point_2& s, const Point_2& t) const {
                    K kernel{};
                    auto d1 = kernel.compute_squared_distance_2_object()(p, q);
                    auto d2 = kernel.compute_squared_distance_2_object()(s, t);

                    return d1 < d2;
                }
            };


            // Used to filter edges of the Delaunay triangulation.
            // They only need to be tested against the vertices of the adjacent triangles
            template <typename K>
            class Is_in_beta_skeleton {
            public:
                using FT = typename K::FT;
                using Point_2 = typename K::Point_2;
                using Segment_2 = typename K::Segment_2;
                using result_type = bool;

                result_type operator()(const Segment_2& e, const Point_2& p, const Point_2& q) const {
                    return both_circles_empty(e.source(), e.target(), p, q);
                }

                result_type operator()(const Point_2& source,const Point_2& target, const Point_2& p, const Point_2& q) const {
                    return both_circles_empty(source, target, p, q);
                }

            private:

                constexpr double beta() const { return  1.17682; }

                constexpr double edge_coefficient() const {
                    // beta^2 / 4 ,used for comparison with a squared radius
                    return 0.346225899999999975;
                };

                result_type both_circles_empty(const Point_2& s,const Point_2& t,
                                               const Point_2& p, const Point_2& q) const
                {
                    const FT& sx = s.x();
                    const FT& sy = s.y();
                    const FT& tx = t.x();
                    const FT& ty = t.y();

                    const FT& px = p.x();
                    const FT& py = p.y();
                    const FT& qx = q.x();
                    const FT& qy = q.y();

                    // Centers of both circumcircle
                    FT x1, y1, x2, y2;

                    const FT sr1 = squared_radiusC2(px, py, sx, sy, tx, ty, x1, y1);
                    const FT sr2 = squared_radiusC2(qx, qy, sx, sy, tx, ty, x2, y2);


                    if(orientationC2(sx, sy, tx, ty, x1, y1) == orientationC2(sx, sy, tx, ty, x2, y2))
                        return false;


                    const FT sd = squared_distanceC2(sx, sy, tx, ty);

                    const FT bound = edge_coefficient() * sd;

                    return (sr1 > bound) && (sr2 > bound);
                }
            };


        public:

            using Compare_squared_radius_2 = Filtered_predicate<Compare_squared_radius<EK>, Compare_squared_radius<FK>, C2E, C2F>;
            using Diamond_test_2 = Filtered_predicate<Diamond_test<EK>, Diamond_test<FK>, C2E, C2F>;
            using Is_in_beta_skeleton_2 = Filtered_predicate<Is_in_beta_skeleton<EK>, Is_in_beta_skeleton<FK>, C2E, C2F>;

            template <typename K>
            using Compute_angle = Compute_pseudo_angle<K>;

            using Compute_polar_angle_2 = Compute_polar_angle<Kernel, Compute_angle>;
            using Construct_dead_sector_2 = Construct_dead_sector<Kernel, Compute_angle>;

            using Angle_less_2 = Angle_less;
            using Less_polar_angle_2 =  Filtered_predicate<Less_polar_angle<EK>, Less_polar_angle<FK>, C2E, C2F>;
            //using Less_distance_2 = Filtered_predicate<Less_distance<EK>, Less_distance<FK>, C2E, C2F>;
            using Less_distance_2 = Less_distance<Kernel>;

            Angle_less_2 angle_less_2_object(const Point_2& p) const {
                return Angle_less_2{p};
            }

            Less_polar_angle_2 less_polar_angle_2_object(const Point_2& p) const {
                return Less_polar_angle_2{p};
            }

            Less_distance_2 less_distance_2_object() const {
                return Less_distance_2{};
            }

            Compare_squared_radius_2 compare_squared_radius_2_object() const {
                return Compare_squared_radius_2{};
            }

            Diamond_test_2 diamond_test_2_object() const {
                return Diamond_test_2{};
            }

            Is_in_beta_skeleton_2 Is_in_beta_skeleton_2_object() const {
                return Is_in_beta_skeleton_2{};
            }

            Construct_dead_sector_2 construct_dead_sector_2_object() const {
                return Construct_dead_sector_2{};
            };

            Compute_polar_angle_2 compute_polar_angle_2_object(const Point_2& p) const {
                return Compute_polar_angle_2{p};
            }

            static constexpr double min_polar_angle() { return Compute_angle<Kernel>::min_polar_angle; }
            static constexpr double max_polar_angle() { return Compute_angle<Kernel>::max_polar_angle; }
        };
    }
}

#endif //MWT_TRAITS_H
