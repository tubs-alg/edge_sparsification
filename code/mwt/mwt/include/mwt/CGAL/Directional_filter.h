#ifndef MWT_DIRECTIONAL_FILTER_H
#define MWT_DIRECTIONAL_FILTER_H

#include "Circular_interval_set.h"

template <typename Traits_>
class Directional_filter {
public:
    using Traits = Traits_;

    using FT = typename Traits::FT;
    using Point_2 = typename Traits::Point_2;
    using Iso_rectangle_2 = typename Traits::Iso_rectangle_2;

    using result_type = bool;

    using Polar_angle_2 = typename Traits::Compute_polar_angle_2;


    Directional_filter() :
            traits{}, dead_sectors{Traits::min_polar_angle(), Traits::max_polar_angle()} {}

    Directional_filter(const Point_2& query) :
            traits{}, query{query}, dead_sectors{Traits::min_polar_angle(), Traits::max_polar_angle()} {}


    result_type operator() (const Point_2& p) const {
        const auto angle = traits.compute_polar_angle_2_object(query);
        return dead_sectors.contains(angle(p));
    }

    result_type operator() (const FT& angle) const {
        return dead_sectors.contains(angle);
    }

    result_type operator() (const FT& l, const FT& r) const {
        return dead_sectors.contains(l, r);
    }

    result_type operator() () const {
        return dead_sectors.completly_covered();
    }

    result_type operator() (const Iso_rectangle_2& bbox) const {
        bool res = bbox_filter_branchless_impl(bbox);
        CGAL_assertion(res == bbox_filter_impl(bbox));
        return res;
    }

    void insert_sector(const std::pair<FT,FT>& pair) {
        insert_sector(pair.first, pair.second);
    }

    void insert_sector(const FT& left, const FT& right) {
        dead_sectors.insert(left, right);
    }

    void clear() {
        dead_sectors.clear();
    }

    void reset(const Point_2& q) {
        clear();
        query = q;
    }

    size_t size() const {
        return dead_sectors.size();
    }

    void print() const {
        dead_sectors.print();
    }

private:

    result_type bbox_filter_impl(const Iso_rectangle_2 &bbox) const {
        const FT& x = query.x();
        const FT& y = query.y();

        FT left;
        FT right;

        const FT& xmin = bbox.xmin();
        const FT& xmax = bbox.xmax();
        const FT& ymin = bbox.ymin();
        const FT& ymax = bbox.ymax();

        const auto angle = traits.compute_polar_angle_2_object(query);

        if(x < xmin) {

            if(y < ymin) {
                left = angle(xmax, ymin);
                right = angle(xmin, ymax);

            } else if (y <= ymax) {
                left = angle(xmin, ymin);
                right = angle(xmin, ymax);

            } else {
                left = angle(xmin, ymin);
                right = angle(xmax, ymax);
            }

        } else if (x <= xmax) {
            if(y < ymin) {
                left = angle(xmax, ymin);
                right = angle(xmin, ymin);

            } else if (y <= ymax) {
                CGAL_assertion(false);
                return false;

            } else {
                left = angle(xmin, ymax);
                right = angle(xmax, ymax);
            }

        } else {
            if(y < ymin) {
                left = angle(xmax, ymax);
                right = angle(xmin, ymin);

            } else if (y <= ymax) {
                left = angle(xmax, ymax);
                right = angle(xmax, ymin);

            } else {
                left = angle(xmin, ymax);
                right = angle(xmax, ymin);
            }
        }

        return dead_sectors.contains(left, right);
    }

    result_type bbox_filter_branchless_impl(const Iso_rectangle_2 &bbox) const {
        const FT& x = query.x();
        const FT& y = query.y();

        const FT& xmin = bbox.xmin();
        const FT& xmax = bbox.xmax();
        const FT& ymin = bbox.ymin();
        const FT& ymax = bbox.ymax();

        const auto angle = traits.compute_polar_angle_2_object(query);

        const std::array<FT, 2> xvalues{xmin, xmax};
        const std::array<FT, 2> yvalues{ymin, ymax};

        const bool a = x < xmin;
        const bool b = x <= xmax;
        const bool c = y < ymin;
        const bool d = y <= ymax;

        const bool l1 = c | (!a & d);
        const bool l2 = (!a) & (!b | !c);
        const bool r1 = (!c) & (!a | !d);
        const bool r2 = a | (b & !c);

        const FT l = angle(xvalues[l1], yvalues[l2]);
        const FT r = angle(xvalues[r1], yvalues[r2]);

        return dead_sectors.contains(l,r);

    }

    const Traits traits;
    Point_2 query;
    Circular_interval_set<FT> dead_sectors;

};


#endif //MWT_DIRECTIONAL_FILTER_H
