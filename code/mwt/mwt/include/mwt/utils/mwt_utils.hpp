#ifndef MWT_MWT_UTILS_HPP
#define MWT_MWT_UTILS_HPP

#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include "CGAL/Minimum_weight_triangulation.h"
#include "CGAL/Lmt_halfedge.h"




namespace utils::mwt
{
    template<typename Point_2, typename Face>
    int connected_components(const Face &face)
    {
       using Graph = boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS>;

        std::map<Point_2, int> point_mapping;
        Graph g;

        for (auto &e : face.hole_boundaries)
        {
            point_mapping.insert(std::make_pair(e->target(), point_mapping.size()));
            point_mapping.insert(std::make_pair(e->twin()->target(), point_mapping.size()));

            boost::add_edge(point_mapping[e->target()], point_mapping[e->twin()->target()], g);
        }

        //for (auto &p : face.isolated_vertices)
        //{
        //    point_mapping.insert(std::make_pair(*p, point_mapping.size()));
        //}

        std::vector<int> component(boost::num_vertices(g));
        return boost::connected_components(g, &component[0]) + face.isolated_vertices.size();
    }

    template <typename Point_2, typename Traits>
    bool instance_is_hard(const std::vector<Point_2> &points, int min_num_of_holes = 1, int min_num_of_hole_edges = 1)
    {
        CGAL::Minimum_weight_triangulation_2<Traits, CGAL::Sequential_tag> mwt{points.begin(), points.end(), true, false, false};

        int num_of_holes = 0;
        int num_of_hole_edges = 0;

        auto count_hole_properties = [&](typename CGAL::Minimum_weight_triangulation_2<Traits, CGAL::Sequential_tag>::Face &face)
        {
            // TODO: hole count does not work
            num_of_holes += 1;
            num_of_hole_edges += face.hole_boundaries.size();

        };

        mwt.register_face_with_holes_callback(count_hole_properties);
        mwt.construct();

        if (num_of_holes >= min_num_of_holes && num_of_hole_edges >= min_num_of_hole_edges)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template <typename Point_2, typename Traits>
    std::vector<Point_2> remove_unnecessary_points(const std::vector<Point_2> & instance) {
        std::vector<Point_2> new_instance;

        CGAL::Minimum_weight_triangulation_2<Traits, CGAL::Sequential_tag> mwt{instance.begin(), instance.end(), true, false, false};

        auto get_hard_points = [&](typename CGAL::Minimum_weight_triangulation_2<Traits, CGAL::Sequential_tag>::Face &face)
        {
            for(auto &e: face.boundary)
            {
                new_instance.push_back(e->target());
            }
            for(auto &p : face.isolated_vertices)
            {
                new_instance.push_back(*p);
            }

            for (auto &e : face.hole_boundaries)
            {
                new_instance.push_back(e->target());
            }
        };

        mwt.register_face_with_holes_callback(get_hard_points);
        mwt.construct();

        std::sort(new_instance.begin(), new_instance.end());
        utils::vector::has_and_remove_duplicates(new_instance);

        return new_instance;
    }
}

#endif //MWT_MWT_UTILS_HPP