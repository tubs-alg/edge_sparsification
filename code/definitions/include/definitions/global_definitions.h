#ifndef SPARSIFICATION_GLOBAL_DEFINITIONS_H
#define SPARSIFICATION_GLOBAL_DEFINITIONS_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/number_utils.h>
#include <CGAL/Bbox_2.h>

#include <boost/graph/adjacency_list.hpp>

using Epick   = CGAL::Exact_predicates_inexact_constructions_kernel;
using Epeck   = CGAL::Exact_predicates_exact_constructions_kernel;
using Kernel  = Epick;
using Point_2 = Kernel::Point_2;
using Vector_2 = Kernel::Vector_2;
using Line_2 = Kernel::Line_2;
using Direction_2 = CGAL::Direction_2<Kernel>;
using Ray_2 = Kernel::Ray_2;
//using Edge = std::pair<Point_2, Point_2>;
using Segment_2 = CGAL::Segment_2<Kernel>;
using Bbox_2 = CGAL::Bbox_2;
using IndexPoint = std::pair<std::size_t, Point_2>;

typedef boost::property<boost::edge_weight_t, double> EdgeWeightProperty;
using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, Point_2, EdgeWeightProperty>;
using vertex_iterator = Graph::vertex_iterator;
using vertex_descriptor = Graph::vertex_descriptor;
using edge_descriptor = Graph::edge_descriptor;

#endif //SPARSIFICATION_GLOBAL_DEFINITIONS_H
