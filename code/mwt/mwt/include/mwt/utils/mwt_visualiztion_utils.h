#ifndef MWT_MWT_VISUALIZTION_UTILS_H
#define MWT_MWT_VISUALIZTION_UTILS_H

#include "include/simplesvg/simple_svg_1.0.0.hpp"

#include "CGAL/Minimum_weight_triangulation.h"
#include "CGAL_definitions.h"

namespace MWT {
    struct BoundingBox {
        double min_x, max_x, min_y, max_y;
        
        BoundingBox(double min_x, double max_x, double min_y, double max_y) : min_x(min_x), max_x(max_x), min_y(min_y), max_y(max_y) {};
        bool empty() {return min_x == 0 && max_x == 0 && min_y == 0 && max_y == 0; };
    };
    
    class MWTVisualizer {

        void set_bb(double min_x, double max_x, double min_y, double max_y)
        {
            bbox_set = true;
            bbox = BoundingBox(min_x, max_x, min_y, max_y);
        }

    private:
        BoundingBox bbox;
        bool bbox_set = false;
    };

    static void visualize_mwt(const std::vector<Point_2> &points, const std::string &outpath = "triangulation.svg", bool simple_faces = true, bool complex_faces = true, BoundingBox bbox = BoundingBox(0, 0, 0, 0))
    {
        if (bbox.empty()) {
            double xmax = std::numeric_limits<double>::lowest(),
                    ymax = std::numeric_limits<double>::lowest(),
                    xmin = std::numeric_limits<double>::max(),
                    ymin = std::numeric_limits<double>::max();


            for (auto &p : points) {
                xmax = std::max(xmax, p.x());
                ymax = std::max(ymax, p.y());
                xmin = std::min(xmin, p.x());
                ymin = std::min(ymin, p.y());
            }
            
            bbox = BoundingBox(xmin, xmax, ymin, ymax);
        }

        double maxdim = 1000;
        double width = bbox.max_x - bbox.min_x;
        double height = bbox.max_y - bbox.min_y;

        double scale = maxdim / std::max(width, height);
        int margin = round(0.05 * maxdim);
        int shape_scale = round(maxdim / 1000);

        svg::Dimensions dim(scale * width + 2 * margin, scale * height + 2 * margin);
        svg::Document doc(outpath, svg::Layout(dim, svg::Layout::BottomLeft));

        for (unsigned long i = 0; i < points.size(); ++i) {
            //doc << svg::Text(svg::Point((points[i].x() - xmin) * scale + margin, (points[i].y() - ymin) * scale + margin), "p" + std::to_string(i), svg::Fill(svg::Color::Black), svg::Font(5 * shape_scale), svg::Stroke(0.01 * shape_scale, svg::Color::Black));
            doc << svg::Circle(svg::Point((points[i].x() - bbox.min_x) * scale + margin, (points[i].y() - bbox.min_y) * scale + margin), 2 * shape_scale, svg::Color::Red);
        }

        CGAL::Minimum_weight_triangulation_2<Traits, CGAL::Sequential_tag> mwt{points.begin(), points.end(), false, simple_faces, complex_faces};
        
        std::vector<Edge> edges;
        mwt.output_mwt(std::back_inserter(edges));

        for (auto & e : edges) {
            doc << svg::Line(svg::Point((e.first.x() - bbox.min_x) * scale + margin, (e.first.y() - bbox.min_y) * scale + margin), svg::Point((e.second.x() - bbox.min_x) * scale + margin, (e.second.y() - bbox.min_y) * scale + margin), svg::Stroke(0.5 * shape_scale , svg::Color::Black));
        }

        doc.save();
    }
}

#endif //MWT_MWT_VISUALIZTION_UTILS_H
