#ifndef MWT_INTERVAL_SET_H
#define MWT_INTERVAL_SET_H

#include <algorithm>
#include <vector>
#include <iterator>

#include <CGAL/assertions.h>

namespace CGAL {

    namespace Minimum_weight_triangulation {

        template<class T>
        struct Interval {
            T left;
            T right;

            Interval(const T& l , const T& r) : left{l}, right{r} {}

            bool operator < (const Interval& rhs) const { return left < rhs.left; }
            bool operator <= (const Interval& rhs) const { return left <= rhs.left; }

            friend std::ostream& operator<< (std::ostream& stream, const Interval& interval) {
                return stream << "[" << interval.left << ", " << interval.right << "]";
            }
        };

        template <class T>
        bool operator < (const Interval<T>& lhs, const T& val) { return lhs.left < val;}
        template <class T>
        bool operator < (const T& val, const Interval<T>& rhs) { return val < rhs.left;}

        template <class T>
        bool operator <= (const Interval<T>& lhs, const T& val) { return lhs.left <= val;}
        template <class T>
        bool operator <= (const T& val, const Interval<T>& rhs) { return val <= rhs.left;}


        template<class T>
        class Interval_set {
        public:

            using Interval_type = Interval<T>;
            using Interval_container = std::vector<Interval_type>;
            using Iterator = typename Interval_container::iterator;
            using Const_iterator = typename Interval_container::const_iterator;


            Interval_set(T min, T max) : min{min}, max{max} {
                CGAL_precondition(min < max);
                intervals.reserve(16);

            }

            Const_iterator begin() const {
                return intervals.cbegin();
            }

            Const_iterator end() const {
                return intervals.cend();
            }

            void insert(const T &left, const T &right) {

                if(intervals.empty()) {
                    intervals.emplace_back(left, right);
                    return;
                }


                //auto lb = std::lower_bound(intervals.begin(), intervals.end(), left);
                auto lb = priv_lower_bound(intervals.begin(), intervals.end(), left);


                if (lb != intervals.begin() && left <= std::prev(lb)->right) {
                    //|__|  |_lb_|
                    //  |___....
                    auto prev = std::prev(lb);

                    if(prev->right < right) {
                        prev->right = right;

                        auto i = lb;
                        for (; i != intervals.end() && i->left <= right; ++i) {
                            prev->right = std::max(i->right, right);
                        }
                        intervals.erase(lb, i);
                    }
                } else {
                    //|__|     |_lb_|
                    //     |__....
                    if (lb == intervals.end() || right < lb->left) {
                        //|__|     |_lb_|
                        //     |__|
                        intervals.emplace(lb, left, right);
                    } else {
                        //|__|     |_lb_|
                        //     |_____......
                        lb->left = left;

                        auto i = lb;
                        for (; i != intervals.end() && i->left <= right; ++i) {
                            lb->right = std::max(i->right, right);
                        }

                        if (i > ++lb)
                            intervals.erase(lb, i);

                    }
                }
            }

            bool empty() const {
                return intervals.empty();
            }

            void clear() {
                intervals.clear();
            }

            bool completly_covered() const {
                return !intervals.empty() && (intervals.front().left <= min) & (intervals.front().right >= max);
            }

            bool contains(const T& left, const T& right) const {
                if (intervals.empty())
                    return false;

                //auto ub = std::upper_bound(intervals.begin(), intervals.end(), left);
                auto ub = priv_upper_bound(intervals.begin(), intervals.end(), left);

                if(ub != intervals.begin()) {
                    --ub;

                    return right <= ub->right;
                }
                else
                    return false;

            }

            // Special function if the interval wraps around
            bool contains(const T& left, const T& right, bool) const {
                if (intervals.empty())
                    return false;

                const auto& b = intervals.back();
                const auto& f = intervals.front();

                return (b.left <= left) & (b.right >= max) &
                        (f.left <= right) & (f.right >= right);
            }

            bool contains(const T& val) const {
                if (intervals.empty())
                    return false;

                //auto ub = std::upper_bound(intervals.begin(), intervals.end(), val);
                auto ub = priv_upper_bound(intervals.begin(), intervals.end(), val);

                if(ub != intervals.begin()) {
                    --ub;

                    return val <= ub->right;
                }
                else
                    return false;

            }

            void print () const {
                for (auto& i : intervals) {
                    std::cout << i <<" ";
                }
                std::cout << std::endl;
            }

            size_t size() const {
                return intervals.size();
            }


        private:

            template <typename It, typename V>
            static It priv_lower_bound(It begin, It end, const V& val) {

                auto len = std::distance(begin, end);

                while (len > 1) {
                    const auto half = len >> 1;
                    auto mid = begin + half;

                    begin = (*mid < val) ? mid : begin;
                    len -= half;
                }

                return begin + (*begin < val);
            }



            template <typename It, typename V>
            static It priv_upper_bound(It begin, It end, const V& val) {

                auto len = std::distance(begin, end);

                while (len > 1) {
                    const auto half = len >> 1;
                    auto mid = begin + half;

                    begin = (*mid <= val) ? mid : begin;
                    len -= half;
                }

                return begin + (*begin <= val);
            }



            const T min;
            const T max;
            Interval_container intervals;

        };

    }
}


#endif //MWT_INTERVAL_SET_H
