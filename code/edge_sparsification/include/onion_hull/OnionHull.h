#ifndef SPARSIFICATION_ONIONHULL_H
#define SPARSIFICATION_ONIONHULL_H

#include <vector>

#include "definitions/global_definitions.h"
#include "edge_sparsification/EdgeSparsification.h"

#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>

typedef CGAL::Convex_hull_traits_adapter_2<Kernel , CGAL::Pointer_property_map<Point_2>::type> Convex_hull_traits_2;

namespace Sparsification {

    class OnionHull {

    public:

        OnionHull(EdgeSparsification & _instance);

        void connectLayers(int variant, int k);

    private:

        std::vector<std::vector<std::size_t>> layers;
        EdgeSparsification & instance;

        void contiguousLayersComplete();

        void contiguousLayersDistance(std::size_t k);

        void contiguousLayersAngle(std::size_t k);

        static double getAngle(Vector_2 & a, Vector_2 & b);

    };

}

#endif //SPARSIFICATION_ONIONHULL_H
