#ifndef MWT_CIRCULAR_INTERVAL_SET_H
#define MWT_CIRCULAR_INTERVAL_SET_H

#include <utility>
#include "Interval_set.h"

template <typename T>
class Circular_interval_set {
public:

    Circular_interval_set(const T min, const T max) : min{min}, max{max}, set{min, max}
        {}

    void insert(const T& left, const T& right) {
        if( left <= right) {
            set.insert(left, right);
        } else{
            // Interval wraps around.
            set.insert(left, max);
            set.insert(min, right);
        }
    }

    bool contains(const T& left, const T& right) const {
        if( left <= right) {
            return set.contains(left, right);
        } else {
            return set.contains(left, right, true);
            //return set.contains(left, max) && set.contains(min, right);
        }
    }

    bool contains(std::pair<T,T> interval) const {
        return contains(interval.first, interval.second);
    }

    bool contains(const T& value) const {
        return set.contains(value);
    }

    void clear() {
        set.clear();
    }

    bool completly_covered() const {
        return set.completly_covered();
    }

    size_t size() const {
        return set.size();
    }

    void print() const {
        set.print();
    }

private:
    using Interval_set = CGAL::Minimum_weight_triangulation::Interval_set<T>;

    const T min;
    const T max;

    Interval_set set;
};


#endif //MWT_CIRCULAR_INTERVAL_SET_H
