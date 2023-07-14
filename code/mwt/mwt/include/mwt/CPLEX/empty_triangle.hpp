#ifndef EMPTY_TRIANGLE_HPP
#define EMPTY_TRIANGLE_HPP
#include <iostream>
#include <vector>
#include <tuple>
#include <memory>


#include "../CGAL/Lmt_halfedge.h"

namespace MWT
{

    // This is a lie. We are using the empty triangle class for triangles which are most likely empty.
    template<typename Traits>
    class EmptyTriangle
    {

    public:
        using Edge = CGAL::Lmt_halfedge<Traits>*;
        using Point_2 = typename Traits::Point_2;

        EmptyTriangle() = default;
        explicit EmptyTriangle(Edge e) {
            assert(e->j && e->i);
            e1 = std::min(e, std::min(e->j, e->i->twin()));
            e3 = std::max(e, std::max(e->j, e->i->twin()));
            if (this->e1 != e && this->e3 != e) {
                this->e2 = e;
            } else if (this->e1 != e->j && this->e3 != e->j) {
                this->e2 = e->j;
            } else {
                this->e2 = e->i->twin();
            }

            this->points = std::make_shared<std::set<Point_2>>();

            this->points->insert(e->source());
            this->points->insert(e->target());
            this->points->insert(e->i->twin()->source());
            this->points->insert(e->i->twin()->target());

            assert(points->size() == 3);
            assert(this->e1 != this->e2 && this->e2 != this->e3 && this->e1 != this->e3);
        }

        Edge e1;
        Edge e2;
        Edge e3;

        std::shared_ptr<std::set<Point_2>> points;

        std::shared_ptr<std::set<Point_2>> get_points()
        {
            return points;
        }

        Point_2 topmost_point()
        {
            Point_2 topmost = *points->begin();

            for(auto &point : *points)
            {
                if(point.y() > topmost.y())
                {
                    topmost = point;
                }
            }

            return topmost;
        }

        Point_2 bottommost_point()
        {
            Point_2 bottommost = *points->begin();

            for(auto &point : *points)
            {
                if(point.y() < bottommost.y())
                {
                    bottommost = point;
                }
            }

            return bottommost;
        }

        Point_2 leftmost_point()
        {
            return *this->points->begin();
        }

        Point_2 rightmost_point()
        {
            return *this->points->rbegin();
        }

        bool consists_of(const Point_2 &p)
        {
            return this->points->find(p) != this->points->end();
        }

        double weight() // wrong weight for antennas
        {
            return  e1->length() +
                    e2->length() +
                    e3->length();

        }

        bool contains_point(const Point_2 &p) {

            using Triangle = typename Traits::Triangle_2;
            if(!this->consists_of(p))
            {
                return Triangle(*this->points->begin(),
                        *std::next(this->points->begin()),
                        *this->points->rbegin()).bounded_side(p) != CGAL::ON_UNBOUNDED_SIDE;
            }

            return false;
        }

        bool operator<(const EmptyTriangle &rhs) const {
            if (e1 < rhs.e1)
                return true;
            if (rhs.e1 < e1)
                return false;
            if (e2 < rhs.e2)
                return true;
            if (rhs.e2 < e2)
                return false;
            return e3 < rhs.e3;
        }

        bool operator==(const EmptyTriangle &rhs) const {
            return e1 == rhs.e1 &&
                   e2 == rhs.e2 &&
                   e3 == rhs.e3;
        }
    };
}


#endif
