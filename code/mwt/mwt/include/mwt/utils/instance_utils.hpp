#ifndef MWT_INSTANCE_UTILS_HPP
#define MWT_INSTANCE_UTILS_HPP

#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include "random_utils.hpp"
#include "CGAL/Minimum_weight_triangulation.h"

#include "include/cppitertools/combinations.hpp"
#include "include/cppitertools/range.hpp"


namespace utils::instance
{
    namespace fs = boost::filesystem;

    template<class Container>
    Container read_file(fs::path file)
    {
        if (!fs::is_regular_file(file))
        {
            throw std::invalid_argument("Not a file: " + file.string());
        }

        fs::ifstream stream{file};

        std::istream_iterator<typename Container::value_type> start{stream};
        std::istream_iterator<typename Container::value_type> end{};

        Container container{start, end};

        return container;
    }

    template <typename Container>
    Container read_hip_file(const fs::path & path) {
        Container points;

        std::ifstream ifs(path.string());

        std::string line;
        while (std::getline(ifs, line))
        {
            if (line[0] == '#')
            {
                continue;
            }

            std::istringstream iss(line);

            double x, y;
            if (!(iss >> x >> y)) {
                throw std::invalid_argument("Invalid format: " + line);
            }

            points.emplace_back(x, y);
        }

        return points;
    }


    template<class Container>
    bool write_file(Container &container, fs::path file)
    {
        if (fs::exists(file))
        {
            throw std::invalid_argument("File exists: " + file.string());
        }

        if (!file.parent_path().empty() && !fs::exists(file.parent_path()))
        {
            fs::create_directories(file.parent_path());
        }

        fs::ofstream stream{file};

        for (auto &x: container)
        {
            stream << std::setprecision(15) << x.x() << " " << x.y() << std::endl;
        }

        return true;
    }

    template<typename Point_2>
    std::vector<Point_2> rotate_instance(const std::vector<Point_2> &points, double rotation_angle = M_PI / 4)
    {
        std::vector<Point_2> result;

        for (auto &p : points)
        {
            double r = std::sqrt(p.x() * p.x() + p.y() * p.y());

            if (r == 0)
            {
                result.push_back(p);
            } else {
                float s = sin(rotation_angle);
                float c = cos(rotation_angle);

                // rotate point
                float xnew = p.x() * c - p.y() * s;
                float ynew = p.x() * s + p.y() * c;

                result.emplace_back(xnew, ynew);
            }

        }

        return result;
    }

    template<typename Point_2>
    bool same_x_or_y_coordinate(std::vector<Point_2> &points)
    {
        for (unsigned long i = 0; i < points.size(); ++i)
        {
            for (unsigned long j = 0; j < i; ++j)
            {
                if (points[i].x() == points[j].x() || points[i].y() == points[j].y())
                {
                    return true;
                }
            }
        }

        return false;
    }

    template<typename Point_2>
    double diameter(const std::vector<Point_2> &points)
    {
        double diameter = 0;

        for (int i = 0; i < points.size(); ++i)
        {
            for (int j = 0; j < i; ++j)
            {
                diameter = std::max(diameter, std::sqrt(CGAL::squared_distance(points[i], points[j])));
            }
        }

        return diameter;
    }

    template<typename Point_2>
    double mininmal_squared_euclidean_distance(const std::vector<Point_2> &points)
    {
        double min_distance = 0;

        for (unsigned long i = 0; i < points.size(); ++i)
        {
            for (unsigned long j = 0; j < i; ++j)
            {
                min_distance = std::min(min_distance, CGAL::squared_distance(points[i], points[j]));
            }
        }

        return min_distance;
    }

    template<typename Point_2>
    void pertub(std::vector<Point_2> &points, double epsilon, int pertubation_index, bool safe_pertubation = false)
    {
        assert(pertubation_index >= 0 && pertubation_index < points.size());


        double x,y;
        do {
            double angle_rad = utils::random::random_double(0, 2 * M_PI);

            x = points[pertubation_index].x() + epsilon * cos(angle_rad);
            y = points[pertubation_index].y() + epsilon * sin(angle_rad);
        } while (safe_pertubation && !(x >= 0 && x <= 1000 && y >= 0 && y <= 1000));

        points[pertubation_index] = Point_2(x, y);
    }

    template<typename Point_2, typename MwtTraits>
    int connected_components(const std::vector<CGAL::Minimum_weight_triangulation_2<MwtTraits>> &edges)
    {
        using Graph = boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS>;

        std::map<Point_2, int> point_mapping;
        Graph g;

        for (auto &e : edges)
        {
            point_mapping.insert(std::make_pair(e->first, point_mapping.size()));
            point_mapping.insert(std::make_pair(e->second, point_mapping.size()));

            boost::add_edge(point_mapping[e->first], point_mapping[e->second], g);
        }

        std::vector<int> component(boost::num_vertices(g));
        return boost::connected_components(g, &component[0]);
    }

    template <typename Kernel>
    std::set<std::set<int>> cocyclic_points(const std::vector<CGAL::Point_2<Kernel>> &points, double epsilon = 10e-3)
    {
        std::set<std::set<int>> cocyclic_points;

        std::cout << "Epsilon: " << epsilon << std::endl;

        int cycles = 0;

        for(auto&& triple : iter::combinations(iter::range(points.size()), 3)) {

            const CGAL::Point_2<Kernel> *p1 = &points[triple[0]];
            const CGAL::Point_2<Kernel> *p2 = &points[triple[1]];
            const CGAL::Point_2<Kernel> *p3 = &points[triple[2]];

            std::set<int> s;

            if (!CGAL::collinear(*p1, *p2, *p3))
            {
                CGAL::Circle_2<Kernel> circle(*p1, *p2, *p3);

                for (unsigned long index = 0; index < points.size(); ++index)
                {
                    if (index != triple[0] && index != triple[1] && index != triple[2]) {
                        if (std::abs(std::sqrt(CGAL::squared_distance(circle.center(), points[index])) -
                                     std::sqrt(circle.squared_radius())) < epsilon)
                        {
                            //std::cout << std::abs(std::sqrt(CGAL::squared_distance(circle.center(), points[index])) -
                            //                      std::sqrt(circle.squared_radius())) << std::endl;
                            //std::cout << "found cocyclic points" << std::endl;
                            //std::cout << *p1 << ", " << *p2 << ", " << *p3 << ", " << *p4 << std::endl;
                            s.insert(index);
                        }
                    }
                }

                if (!s.empty())
                {
                    ++cycles;
                    s.insert(triple[0]);
                    s.insert(triple[1]);
                    s.insert(triple[2]);
                    cocyclic_points.insert(s);
                }
            }
        }

        return cocyclic_points;
    }

    template <typename Kernel>
    void normalize(std::vector<CGAL::Point_2<Kernel>> &points, double xdim = 1000, double ydim = 1000, bool keep_ratio = true)
    {
        double xmax = std::numeric_limits<double>::lowest(),
                ymax = std::numeric_limits<double>::lowest(),
                xmin = std::numeric_limits<double>::max(),
                ymin = std::numeric_limits<double>::max();

        for (auto & p : points)
        {
            xmax = std::max(xmax, std::max(p.x(), p.x()));
            ymax = std::max(ymax, std::max(p.y(), p.y()));
            xmin = std::min(xmin, std::min(p.x(), p.x()));
            ymin = std::min(ymin, std::min(p.y(), p.y()));
        }

        double xscale = xdim / (xmax - xmin);
        double yscale = ydim / (ymax - ymin);

        if (keep_ratio)
        {
            double scale = std::min(xscale, yscale);
            xscale = scale;
            yscale = scale;
        }

        for (auto &p : points)
        {
            p = CGAL::Point_2<Kernel>((p.x() - xmin) * xscale, (p.y() - ymin) * yscale);
        }
    }


    template<typename Point_2>
    std::vector<Point_2> create_regular_polygon(int size, double radius, bool centered_vertex = true, double xoffset = 0, double yoffset = 0)
    {
        std::vector<Point_2> instance;

        if (centered_vertex) instance.emplace_back(0 + xoffset, 0 + yoffset);

        double angle = 2 * M_PI / size;

        for (int i = 0; i < size; ++i)
        {
            double x = radius * cos(i * angle);
            double y = radius * sin(i * angle);

            instance.emplace_back(x + xoffset, y + yoffset);
        }

        return instance;
    }
}

#endif //MWT_INSTANCE_UTILS_HPP
