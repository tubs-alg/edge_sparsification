#ifndef MWT_STATS_H
#define MWT_STATS_H

#ifdef USING_STATS

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <chrono>
#include <cmath>

namespace bacc = boost::accumulators;

struct Mwt_stats {

    struct {

        struct {
            std::chrono::nanoseconds tree_build;
            std::chrono::nanoseconds total;
        } time;

        bacc::accumulator_set<std::size_t, bacc::stats<
                bacc::tag::min,
                bacc::tag::max,
                bacc::tag::mean,
                bacc::tag::variance>
        > iterations;

        size_t early_exits{0};
        size_t candidates_after;

    } Diamond_test;

    struct {
        struct {
            std::chrono::nanoseconds init;
            std::chrono::nanoseconds lmt_loop;
            std::chrono::nanoseconds intersection_test;
            std::chrono::nanoseconds advanced_lmt_loop;
            std::chrono::nanoseconds total;
        } time;

        struct {
            size_t after_lmt_loop;
            size_t after_advanced_lmt_loop;
        } certain_edges;

        struct {
            size_t after_lmt_loop;
            size_t after_advanced_lmt_loop;
        } possible_edges;

    } Lmt_skeleton;

    struct {
        struct {
            std::chrono::nanoseconds total;
        } time;

        struct {
            size_t simple{0};
            size_t total{0};
        } faces;

    } Triangulation;

    size_t Mwt_size;


    friend std::ostream& operator<< (std::ostream& stream, const Mwt_stats& stats) {
        stream << "Diamond test: \n\t"
               << "Tree build: " << stats.Diamond_test.time.tree_build.count() << "ns\n\t"
               << "Total: " << stats.Diamond_test.time.total.count() << "ns\n\t"
               << "Candidates remaining: " << stats.Diamond_test.candidates_after << "\n\t"
               << "Neighbors visited: \n\t\t"
               << "Mean: " << bacc::mean(stats.Diamond_test.iterations) << "\n\t\t"
               << "Std-dev: " << std::sqrt(bacc::variance(stats.Diamond_test.iterations)) << "\n\t\t"
               << "Variance: " << bacc::variance(stats.Diamond_test.iterations) << "\n\t\t"
               << "Min: " << bacc::min(stats.Diamond_test.iterations) << "\n\t\t"
               << "Max: " << bacc::max(stats.Diamond_test.iterations) << "\n\t"
               << "Full circle: " << stats.Diamond_test.early_exits << std::endl;



        stream << "Lmt-Skeleton: \n\t"
               << "Init: " << stats.Lmt_skeleton.time.init.count() << "ns\n\t"
               << "Lmt-Loop: " << stats.Lmt_skeleton.time.lmt_loop.count() << "ns\n\t"
               << "Intersection_test: " << stats.Lmt_skeleton.time.intersection_test.count() << "ns\n\t"
               << "Candidates remaining: " << stats.Lmt_skeleton.possible_edges.after_lmt_loop << "\n\t"
               << "Skeleton size: " << stats.Lmt_skeleton.certain_edges.after_lmt_loop << "\n\t"
               << "Advanced: " << stats.Lmt_skeleton.time.advanced_lmt_loop.count() << "ns\n\t"
               << "Candidates remaining: " << stats.Lmt_skeleton.possible_edges.after_advanced_lmt_loop << "\n\t"
               << "Skeleton size: " << stats.Lmt_skeleton.certain_edges.after_advanced_lmt_loop << "\n\t"
               << "Total: " << stats.Lmt_skeleton.time.total.count() << "ns" << std::endl;

        stream << "Face triangulation: \n\t"
               << "Simple faces: " << stats.Triangulation.faces.simple << "\n\t"
               << "Total faces: " << stats.Triangulation.faces.total << "\n\t"
               << "Total: " << stats.Triangulation.time.total.count() << "ns" << std::endl;

        stream << "MWT-Size: " << stats.Mwt_size;

        return stream;
    }
};
#else

struct Mwt_stats {

    size_t Mwt_size;

    friend std::ostream& operator<< (std::ostream& stream, const Mwt_stats& stats) {
        (void)stats; // supress unused-parameter warning
        stream << "Statistics are not enabled" << std::endl;
        return stream;
    }

};



#endif

#endif //MWT_STATS_H
