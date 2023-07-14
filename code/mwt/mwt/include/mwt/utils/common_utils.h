//
// Created by Yannic Lieder on 2019-05-02.
//

#ifndef MWT_COMMON_UTILS_H
#define MWT_COMMON_UTILS_H

namespace utils::common {
    template <typename T>
    const T& min(const T& a, const T& b, const T& c) {
        return std::min(std::min(a, b), c);
    }

    template <typename T>
    const T& middle(const T& a, const T& b, const T& c) {

        auto leq_rel = [](const T& a, const T& b, const T& c) -> bool {
            return !(b < a || c < b); // equivalent to a <= b && b <= c
        };

        if (leq_rel(a, b, c) || leq_rel(c, b, a)) {
            return b;
        } else if (leq_rel(b, a, c) || leq_rel(c, a, b)) {
            return a;
        } else {
            return c;
        }
    }

    template <typename T>
    const T& max(const T& a, const T& b, const T& c) {
        return std::max(std::max(a, b), c);
    }
}

#endif //MWT_COMMON_UTILS_H
