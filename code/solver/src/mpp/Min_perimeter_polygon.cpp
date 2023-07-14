#include "mpp/Min_perimeter_polygon.h"

namespace mpp {

    void print_ccb(Arrangement_2::Ccb_halfedge_const_circulator circ, std::map<Point, std::size_t> point_map) {
        Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
        std::cout << "(" << point_map[curr->source()->point()] << ")";
        do {
            std::cout << " [" << curr->curve() << "] "
                      << "(" << point_map[curr->target()->point()] << ")";
        } while (++curr != circ);
        std::cout << std::endl;
    }

    void print_face(Arrangement_2::Face_const_handle f, std::map<Point, std::size_t> point_map) {
        // Print the outer boundary.
        if (f->is_unbounded())
            std::cout << "Unbounded face. " << std::endl;
        else {
            std::cout << "Outer boundary: ";
            print_ccb(f->outer_ccb(), point_map);
        }
        // Print the boundary of each of the holes.
        Arrangement_2::Hole_const_iterator hi;
        int index = 1;
        for (hi = f->holes_begin(); hi != f->holes_end(); ++hi, ++index) {
            std::cout << " Hole #" << index << ": ";
            print_ccb(*hi, point_map);
        }
        // Print the isolated vertices.
        Arrangement_2::Isolated_vertex_const_iterator iv;
        for (iv = f->isolated_vertices_begin(), index = 1;
             iv != f->isolated_vertices_end(); ++iv, ++index) {
            std::cout << " Isolated vertex #" << index << ": "
                      << "(" << point_map[iv->point()] << ")" << std::endl;
        }
    }

    void print_arrangement(const Arrangement_2 &arr, const std::map<Point, std::size_t> &point_map) {
        // Print the arrangement vertices.
        Arrangement_2::Vertex_const_iterator vit;
        std::cout << arr.number_of_vertices() << " vertices:" << std::endl;
        for (vit = arr.vertices_begin(); vit != arr.vertices_end(); ++vit) {
            std::cout << "(" << vit->point() << ")";
            if (vit->is_isolated())
                std::cout << " - Isolated." << std::endl;
            else
                std::cout << " - degree " << vit->degree() << std::endl;
        }
        // Print the arrangement edges.
        Arrangement_2::Edge_const_iterator eit;
        std::cout << arr.number_of_edges() << " edges:" << std::endl;
        for (eit = arr.edges_begin(); eit != arr.edges_end(); ++eit)
            std::cout << "[" << eit->curve() << "]" << std::endl;
        // Print the arrangement faces.
        Arrangement_2::Face_const_iterator fit;
        std::cout << arr.number_of_faces() << " faces:" << std::endl;
        for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit)
            print_face(fit, point_map);
    }

    std::string solver_type_str(Solver_type type) {
        switch (type) {
            case MPP:
                return "Mpp";
            default:
                return "";
        }
    }

    LazyCycle::LazyCycle(std::map<edge, GRBVar> &xv, Solver::Instance &_instance) : instance(_instance) {
        xvars = xv;
        n = boost::num_vertices(instance.g);
        cbCnt = 0;
        cycleCutCnt = 0;
        glueCutCnt = 0;
        tailCutCnt = 0;
        hihCutCnt = 0;
        std::vector<std::size_t> indices(instance.points.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::vector<std::size_t> out;
        CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                            Convex_hull_traits_2(CGAL::make_property_map(instance.points)));
        ch_indices = std::vector<bool>(n, false);
        for (auto ch_i : out) {
            ch_indices[ch_i] = true;
        }
        for (auto i : indices) {
            if (!ch_indices[i]) {
                for (int ch_i = 0; ch_i < out.size(); ++ch_i) {
                    Line_2 line{instance.points[out[ch_i]], instance.points[out[(ch_i + 1) % out.size()]]};
                    if (line.has_on(instance.points[i])) {
                        ch_indices[i] = true;
                        break;
                    }
                }
            }
            Point p{instance.points[i].x(), instance.points[i].y()};
            points.emplace_back(p);
            point_map[p] = i;
        }
    }

    void LazyCycle::callback() {
        try {
            if (where == GRB_CB_MIPSOL) {
                cbCnt++;

//                plot();
//                std::cout << "MIPSOL CB" << std::endl;

                // Initializing the arrangement with all edges of the current solution
                Arrangement_2 arr;
                auto edges = boost::edges(instance.g);
                for (auto e = edges.first; e != edges.second; ++e) {
                    auto i = boost::source(*e, instance.g);
                    auto j = boost::target(*e, instance.g);
                    if (getSolution(xvars[edge(i, j)]) > 0.5) {
                        Arr_Segment_2 seg{points[i], points[j]};
                        try {
                            CGAL::insert(arr, seg);
                        } catch (CGAL::Failure_exception &e) {
                            std::cout << "ARR NOT INSERTED: " << i << " - " << j << std::endl;
                            std::cout << e.what() << std::endl;
                        }
                    }
                }

                std::vector<TDS::Vertex_handle> cdt_point_vh = std::vector<TDS::Vertex_handle>(
                        n); // to access the cdt vertex handles by index
                ConstrainedDelaunay cdt;
                for (auto &e : arr.edge_handles()) {
                    // insert the arrangement edges as constraints to the cdt
                    cdt.insert_constraint(e->source()->point(), e->target()->point());
                }
                for (auto &v : boost::make_iterator_range(cdt.finite_vertices_begin(), cdt.finite_vertices_end())) {
                    if (point_map.count(v.point())) {
                        v.id() = point_map.at(v.point());
                        cdt_point_vh[v.id()] = v.handle();
                    } else {
                        // if the vertex is an edge intersection in the arrangement
                        v.id() = -1;
                    }
                }

//                std::cout << "ARRANGEMENT" << std::endl;

                // store all faces with their indices, cnt of ch points and whether they are degenerated
                std::vector<std::tuple<Arrangement_2::Face_const_handle, std::vector<std::size_t>, std::size_t, bool>> faces{};
                std::size_t faces_with_ch_points = 0;
                for (Arrangement_2::Face_const_iterator fit = arr.faces_begin(); fit != arr.faces_end(); ++fit) {
                    for (auto hole = fit->holes_begin(); hole != fit->holes_end(); ++hole) {
                        // completely degenerated 'faces' are stored as holes in the arrangement
                        // what about faces with antennas?
                        if ((*hole)->face() == (*hole)->twin()->face()) {
                            std::vector<std::size_t> indices;
                            std::unordered_set<std::size_t> seen;
                            std::size_t ch_points_cnt = 0;
                            Arrangement_2::Ccb_halfedge_const_circulator circ = *hole;
                            Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
                            do {
                                if (point_map.count(curr->source()->point())) {
                                    auto idx = point_map.at(curr->source()->point());
                                    if (seen.count(idx) == 0) {
                                        seen.insert(idx);
                                        indices.push_back(idx);
                                        if (ch_indices[idx]) ch_points_cnt++;
                                    }
                                }
                            } while (++curr != circ);
                            if (ch_points_cnt) faces_with_ch_points++;
                            faces.emplace_back((*hole)->face(), indices, ch_points_cnt, true);
                        }
                    }
                    if (fit->is_unbounded()) continue;
                    std::vector<std::size_t> indices;
                    std::size_t ch_points_cnt = 0;
                    std::size_t points_cnt = 0;
                    Arrangement_2::Ccb_halfedge_const_circulator circ = fit->outer_ccb();
                    Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
                    do {
                        points_cnt++;
                        if (point_map.count(curr->source()->point())) {
                            auto idx = point_map.at(curr->source()->point());
                            indices.push_back(idx);
                            if (ch_indices[idx]) ch_points_cnt++;
                        }
                    } while (++curr != circ);
                    if (ch_points_cnt) faces_with_ch_points++;
                    if (points_cnt == indices.size()) {
                        // faces where this is not true can be easily shortened by removing a crossing
                        faces.emplace_back(fit, indices, ch_points_cnt, false);
                    }
                }

//                std::cout << "FACES & Holes" << std::endl;

                // cycle cut if degenerated face or #ch-points > 0 and < n
                for (auto const&[f, indices, ch_points_cnt, degenerated] : faces) {
                    if ((degenerated) || (faces_with_ch_points > 1 && ch_points_cnt)) {
                        if (indices.size() <= (2 * n + 1) / 3) {
                            add_constraint_type_1(indices);
                        } else {
                            add_constraint_type_2(indices);
                        }
                        cycleCutCnt++;
                    }
                }

//                std::cout << "CYCLE CUT" << std::endl;

                //glue cut if there are two or more cycles containing ch-points
                if (arr.unbounded_face()->number_of_holes() > 1 && faces_with_ch_points > 1) {
                    std::vector<boost::queue<TDS::Edge>> bfs_q;
                    std::vector<std::set<TDS::Edge>> visited;
                    TDS::Face_circulator ch_circ(cdt.infinite_vertex());
                    TDS::Face_circulator ch_curr = ch_circ;
                    // start from all unconstrained convex hull edges
                    do {
                        int inf_idx = (*ch_curr).index(cdt.infinite_vertex());
                        if (!(*ch_curr).is_constrained(inf_idx)) {
                            boost::queue<TDS::Edge> q;
                            std::set<TDS::Edge> v;
                            q.push(std::make_pair(ch_curr, inf_idx)); // edge to queue

                            v.insert(std::make_pair(ch_curr, inf_idx)); // edge seen
                            auto s = ch_curr->vertex(ch_curr->cw(inf_idx));
                            auto neighbor = ch_curr->neighbor(inf_idx);
                            v.insert(std::make_pair(neighbor,
                                                    neighbor->cw(neighbor->index(s)))); // twin edge in cdt seen

                            bfs_q.push_back(q);
                            visited.push_back(v);
                        }
                    } while (++ch_curr != ch_circ);

                    // indices of the queues that match
                    int ch_i = 0;
                    int ch_j = 0;
                    TDS::Edge connectingEdge;
                    std::vector<std::map<TDS::Edge, TDS::Edge>> backtrace = std::vector<std::map<TDS::Edge, TDS::Edge>>(
                            bfs_q.size());
                    bool connected = false;
                    std::size_t emptyCnt = 0;
                    while (!connected && emptyCnt < bfs_q.size()) {
                        emptyCnt = 0;
                        for (int i = 0; i < bfs_q.size(); ++i) {
                            if (bfs_q[i].empty()) {
                                emptyCnt++;
                                continue;
                            }
                            auto &e = bfs_q[i].front();
                            auto &[face, idx] = e;
                            TDS::Vertex_handle s = face->vertex(face->cw(idx));
                            TDS::Vertex_handle t = face->vertex(face->ccw(idx));
                            TDS::Face_handle face_neighbor = face->neighbor(idx);

                            for (int j = 0; j < bfs_q.size(); ++j) {
                                if (i == j) continue;
                                if (visited[j].count(e)) {
                                    ch_i = i;
                                    ch_j = j;
                                    connectingEdge = e;
                                    connected = true;
                                    break;
                                }
                            }
                            if (connected) break;

                            std::vector<TDS::Edge> next_edges;
                            next_edges.emplace_back(
                                    std::make_pair(face_neighbor, face_neighbor->index(s)));
                            next_edges.emplace_back(
                                    std::make_pair(face_neighbor, face_neighbor->index(t)));
                            for (auto &nextEdge : next_edges) {
                                if (!cdt.is_constrained(nextEdge) && visited[i].count(nextEdge) == 0) {
                                    // not constrained and not already seen
                                    auto &[faceNext, idxNext] = nextEdge;
                                    visited[i].insert(nextEdge); // next edge
                                    auto next_s = faceNext->vertex(faceNext->cw(idxNext));
                                    auto next_neighbor = faceNext->neighbor(idxNext);
                                    visited[i].insert(std::make_pair(next_neighbor, next_neighbor->cw(
                                            next_neighbor->index(next_s)))); // twin
                                    backtrace[i][nextEdge] = e;
                                    bfs_q[i].push(nextEdge);
                                }
                            }
                            bfs_q[i].pop();
                        }
                    }
                    if (ch_i != ch_j) {
                        // compute the glue cut curve
                        std::vector<Point> curve;

                        // backtracing the first bfs queue
                        TDS::Edge curr = connectingEdge;
                        curve.emplace_back(CGAL::midpoint(cdt.segment(curr).source(), cdt.segment(curr).target()));
                        do {
                            curr = backtrace[ch_i].at(curr);
                            auto seg = cdt.segment(curr);
                            curve.emplace_back(CGAL::midpoint(seg.source(), seg.target()));
                        } while (backtrace[ch_i].count(curr));
                        std::reverse(curve.begin(), curve.end());

                        // backtracing the second bfs queue
                        auto s = connectingEdge.first->vertex(connectingEdge.first->cw(connectingEdge.second));
                        auto connectingEdge_neighbor = connectingEdge.first->neighbor(connectingEdge.second);
                        curr = std::make_pair(connectingEdge_neighbor,
                                              connectingEdge_neighbor->cw(connectingEdge_neighbor->index(s)));
                        do {
                            curr = backtrace[ch_j].at(curr);
                            auto seg = cdt.segment(curr);
                            curve.emplace_back(CGAL::midpoint(seg.source(), seg.target()));
                        } while (backtrace[ch_j].count(curr));

                        add_glue_cut(curve);
                        glueCutCnt++;
                    }
                }

//                std::cout << "GLUE CUT" << std::endl;

                // tail cut for outer components not containing any ch-points
                if (faces_with_ch_points < arr.unbounded_face()->number_of_holes()) {
                    for (auto const&[f, indices, ch_points_cnt, degenerated] : faces) {
                        std::vector<boost::queue<TDS::Edge>> bfs_q;
                        std::vector<std::set<TDS::Edge>> visited;
                        if (!degenerated && ch_points_cnt == 0 && f->outer_ccb()->twin()->face()->is_unbounded()) {
                            for (auto i : indices) {
                                // incident constraints of the cycle vertices
                                std::vector<TDS::Edge> out;
                                cdt.incident_constraints(cdt_point_vh[i], std::back_inserter(out));
                                for (auto const &[ef, ei] : out) {
                                    if (ef->vertex(ef->cw(ei))->id() < ef->vertex(ef->ccw(ei))->id()) {
                                        boost::queue<TDS::Edge> q;
                                        std::set<TDS::Edge> v;
                                        q.push(std::make_pair(ef, ei)); // edge to queue
                                        auto s = ef->vertex(ef->cw(ei));
                                        auto neighbor = ef->neighbor(ei);
                                        bfs_q.push_back(q);
                                        v.insert(std::make_pair(ef, ei)); // edge seen
                                        v.insert(std::make_pair(neighbor,
                                                                neighbor->cw(neighbor->index(s)))); // twin edge seen
                                        visited.push_back(v);
                                    }
                                }
                            }
                        }

                        if (!bfs_q.empty()) {
                            int bfs_index;
                            TDS::Edge connectingEdge;
                            std::vector<std::map<TDS::Edge, TDS::Edge>> backtrace = std::vector<std::map<TDS::Edge, TDS::Edge>>(
                                    bfs_q.size());
                            bool connected = false;
                            std::size_t emptyCnt = 0;
                            while (!connected && emptyCnt < bfs_q.size()) {
                                emptyCnt = 0;
                                for (int i = 0; i < bfs_q.size(); ++i) {
                                    if (bfs_q[i].empty()) {
                                        emptyCnt++;
                                        continue;
                                    }
                                    auto &e = bfs_q[i].front();
                                    auto &[face, idx] = e;
                                    TDS::Vertex_handle s = face->vertex(face->cw(idx));
                                    TDS::Vertex_handle t = face->vertex(face->ccw(idx));
                                    TDS::Face_handle face_neighbor = face->neighbor(idx);
                                    if (face_neighbor->has_vertex(cdt.infinite_vertex())) {
                                        // e is on ch
                                        bfs_index = i;
                                        connectingEdge = e;
                                        connected = true;
                                        break;
                                    }

                                    std::vector<TDS::Edge> next_edges;
                                    // we dont know if we start from inside the current face
                                    // so we move in both directions
                                    next_edges.emplace_back(
                                            std::make_pair(face_neighbor, face_neighbor->index(s)));
                                    next_edges.emplace_back(
                                            std::make_pair(face_neighbor, face_neighbor->index(t)));
                                    next_edges.emplace_back(std::make_pair(face, face->cw(idx)));
                                    next_edges.emplace_back(std::make_pair(face, face->ccw(idx)));
                                    for (auto &nextEdge : next_edges) {
                                        if (!cdt.is_constrained(nextEdge) && visited[i].count(nextEdge) == 0) {
                                            auto &[faceNext, idxNext] = nextEdge;
                                            visited[i].insert(nextEdge); // next edge seen
                                            auto next_s = faceNext->vertex(faceNext->cw(idxNext));
                                            auto next_neighbor = faceNext->neighbor(idxNext);
                                            visited[i].insert(std::make_pair(next_neighbor, next_neighbor->cw(
                                                    next_neighbor->index(next_s)))); // twin edge seen
                                            backtrace[i][nextEdge] = e;
                                            bfs_q[i].push(nextEdge); // next edge to queue
                                        }
                                    }
                                    bfs_q[i].pop();
                                }
                            }

                            if (connected) {
                                // compute the tail cut curve
                                std::vector<Point> curve;
                                TDS::Edge curr = connectingEdge;
                                curve.emplace_back(
                                        CGAL::midpoint(cdt.segment(curr).source(), cdt.segment(curr).target()));
                                do {
                                    curr = backtrace[bfs_index].at(curr);
                                    auto seg = cdt.segment(curr);
                                    curve.emplace_back(CGAL::midpoint(seg.source(), seg.target()));
                                } while (backtrace[bfs_index].count(curr));

//                                plot_with_cut(curve);
                                add_tail_cut(curve, indices);
                                tailCutCnt++;
                            }
                        }
                    }
                }

//                std::cout << "TAIL CUT" << std::endl;

                // hole-in-hole cut
                // are there any good conditions before looking for holes in holes?
                for (auto const&[f, indices, ch_points_cnt, degenerated] : faces) {
                    if (f->has_outer_ccb()) {
                        Arrangement_2::Ccb_halfedge_const_circulator circ = f->outer_ccb();
                        Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
                        bool outer_unbounded = false;
                        do {
                            if (curr->twin()->face()->is_unbounded()) {
                                outer_unbounded = true;
                                break;
                            }
                        } while (++curr != circ);
                        if (outer_unbounded) continue; // at least one of the outer faces is unbounded
                        if (f->number_of_holes() > 0) {
                            if (indices.size() <= (2 * n + 1) / 3) {
                                add_constraint_type_1(indices);
                            } else {
                                add_constraint_type_2(indices);
                            }
                            cycleCutCnt++;
                        }
                        auto outer_face = f->outer_ccb()->twin()->face();
                        if (outer_face->has_outer_ccb()) {
                            auto outer_outer_face = outer_face->outer_ccb()->twin()->face();
                            if (outer_outer_face->has_outer_ccb() &&
                                outer_outer_face->outer_ccb()->twin()->face()->is_unbounded()) {
                                std::vector<boost::queue<std::tuple<TDS::Edge, std::size_t, std::size_t>>> bfs_q;
                                std::vector<std::set<TDS::Edge>> visited;
                                std::set<TDS::Edge> visited_sample;
                                for (auto i : indices) {
                                    std::vector<TDS::Edge> out;
                                    cdt.incident_constraints(cdt_point_vh[i], std::back_inserter(out));
                                    for (auto const &[ef, ei] : out) {
                                        if (ef->vertex(ef->cw(ei))->id() <= ef->vertex(ef->ccw(ei))->id()) {
                                            boost::queue<std::tuple<TDS::Edge, std::size_t, std::size_t>> q;
                                            q.push(std::make_tuple(std::make_pair(ef, ei), 0,
                                                                   0)); // edge to queue with total edge cnt and constrained edge cnt
                                            bfs_q.push_back(q);
                                            visited_sample.insert(std::make_pair(ef, ei)); // edge seen
                                            auto s = ef->vertex(ef->cw(ei));
                                            auto neighbor = ef->neighbor(ei);
                                            visited_sample.insert(std::make_pair(neighbor, neighbor->cw(
                                                    neighbor->index(s)))); // twin edge seen
                                        }
                                    }
                                }
                                for (int i = 0; i < bfs_q.size(); ++i) {
                                    visited.emplace_back(visited_sample);
                                }

                                if (!bfs_q.empty()) {
                                    int bfs_index;
                                    std::size_t minConstrEdges = std::numeric_limits<std::size_t>::max();
                                    std::size_t minEdges = std::numeric_limits<std::size_t>::max();
                                    TDS::Edge ch_edge;
                                    std::vector<std::map<TDS::Edge, TDS::Edge>> backtrace = std::vector<std::map<TDS::Edge, TDS::Edge>>(
                                            bfs_q.size());
                                    std::size_t emptyCnt = 0;
                                    while (emptyCnt < bfs_q.size()) {
                                        emptyCnt = 0;
                                        for (int i = 0; i < bfs_q.size(); ++i) {
                                            if (bfs_q[i].empty()) {
                                                emptyCnt++;
                                                continue;
                                            }
                                            auto &[e, edgeCnt, constrEdgeCnt] = bfs_q[i].front();
                                            auto &[face, idx] = e;
                                            TDS::Vertex_handle s = face->vertex(face->cw(idx));
                                            TDS::Vertex_handle t = face->vertex(face->ccw(idx));
                                            TDS::Face_handle face_neighbor = face->neighbor(idx);
                                            if (face_neighbor->has_vertex(cdt.infinite_vertex())) {
                                                // ch edge
                                                if (constrEdgeCnt < minConstrEdges || (constrEdgeCnt <= minConstrEdges && edgeCnt < minEdges)) {
                                                    // shorter cut
                                                    bfs_index = i;
                                                    ch_edge = e;
                                                    minConstrEdges = constrEdgeCnt;
                                                    minEdges = edgeCnt;
                                                }
                                                bfs_q[i].pop();
                                                continue;
                                            }

                                            std::vector<TDS::Edge> next_edges;
                                            next_edges.emplace_back(
                                                    std::make_pair(face_neighbor, face_neighbor->index(s)));
                                            next_edges.emplace_back(
                                                    std::make_pair(face_neighbor, face_neighbor->index(t)));
                                            next_edges.emplace_back(std::make_pair(face, face->cw(idx)));
                                            next_edges.emplace_back(std::make_pair(face, face->ccw(idx)));
                                            for (auto &nextEdge : next_edges) {
                                                if (visited[i].count(nextEdge) == 0) {
                                                    auto &[faceNext, idxNext] = nextEdge;
                                                    auto next_s = faceNext->vertex(faceNext->cw(idxNext));
                                                    auto next_neighbor = faceNext->neighbor(idxNext);
                                                    // push edge with cnts
                                                    if (cdt.is_constrained(nextEdge)) {
                                                        if (constrEdgeCnt + 1 > 2) {
                                                            continue;
                                                        }
                                                        bfs_q[i].push(std::make_tuple(nextEdge, edgeCnt + 1,
                                                                                      constrEdgeCnt + 1));
                                                    } else {
                                                        if (constrEdgeCnt == 2 && edgeCnt + 1 > minEdges) {
                                                            continue;
                                                        }
                                                        bfs_q[i].push(
                                                                std::make_tuple(nextEdge, edgeCnt + 1,
                                                                                constrEdgeCnt));
                                                    }
                                                    visited[i].insert(nextEdge); // next edge seen
                                                    visited[i].insert(std::make_pair(next_neighbor,
                                                                                     next_neighbor->cw(
                                                                                             next_neighbor->index(
                                                                                                     next_s)))); // next edge twin seen
                                                    backtrace[i][nextEdge] = e;
                                                }
                                            }
                                            bfs_q[i].pop();
                                        }
                                    }

                                    if (minConstrEdges == 2) {
                                        std::vector<Point> curve;
                                        TDS::Edge curr = ch_edge;
                                        auto seg = cdt.segment(curr);
                                        curve.emplace_back(CGAL::midpoint(seg.source(), seg.target()));
                                        do {
                                            curr = backtrace[bfs_index].at(curr);
                                            seg = cdt.segment(curr);
                                            curve.emplace_back(CGAL::midpoint(seg.source(), seg.target()));
                                        } while (backtrace[bfs_index].count(curr));
//                                        plot_with_cut(curve);
                                        add_hih_cut(curve, indices);
                                        hihCutCnt++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } catch (GRBException &e) {
            std::cout << "Error during callback" << std::endl;
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (CGAL::Failure_exception &e) {
            std::cout << "CGAL Error during callback" << std::endl;
            std::cout << e.what() << std::endl;
        } catch (...) {
            std::cout << "Error during callback" << std::endl;
        }
    }

    void LazyCycle::add_constraint_type_1(const std::vector<std::size_t> &cycle) {
        std::vector<bool> currentCycle = std::vector<bool>(n, false);
        auto len = cycle.size();
        for (int i = 0; i < len; ++i) currentCycle[cycle[i]] = true;
        std::unordered_set<edge, boost::hash<edge>> expr_edges;
        for (int i = 0; i < len; ++i) {
            for (auto e : boost::make_iterator_range(boost::out_edges(cycle[i], instance.g))) {
                auto target = boost::target(e, instance.g);
                if (!currentCycle[target]) {
                    continue;
                } else {
                    if (cycle[i] < target) {
                        expr_edges.insert(edge(cycle[i], target));
                    } else {
                        expr_edges.insert(edge(target, cycle[i]));
                    }
                }
            }
        }
        GRBLinExpr expr = 0;
        for (auto e : expr_edges) {
            expr += xvars[e];
        }
        addLazy(expr <= len - 1);
    }

    void LazyCycle::add_constraint_type_2(const std::vector<std::size_t> &cycle) {
        std::vector<bool> currentCycle = std::vector<bool>(n, false);
        auto len = cycle.size();
        for (int i = 0; i < len; ++i) currentCycle[cycle[i]] = true;
        GRBLinExpr expr = 0;
        for (int i = 0; i < len; ++i) {
            for (auto e : boost::make_iterator_range(boost::out_edges(cycle[i], instance.g))) {
                auto target = boost::target(e, instance.g);
                if (currentCycle[target]) {
                    continue;
                } else {
                    expr += xvars[edge(cycle[i], target)];
                }
            }
        }
        addLazy(expr >= 2);
    }

    void LazyCycle::add_glue_cut(const std::vector<Point> &cut) {
        try {
            std::vector<Segment> segments;
            for (int j, i = 0; i < cut.size() - 1; i = j) {
                j = i + 1;
                if (j < cut.size() - 1) {
                    while (j < cut.size() - 1 && CGAL::collinear(cut[i], cut[j], cut[j + 1])) j++;
                }
                segments.emplace_back(Segment{cut[i], cut[j]});
            }

            GRBLinExpr expr = 0;

            auto edges = boost::edges(instance.g);
            for (auto e = edges.first; e != edges.second; ++e) {
                auto i = boost::source(*e, instance.g);
                auto j = boost::target(*e, instance.g);
                Segment segment2{points[i], points[j]};
                for (auto &s : segments) {
                    if (CGAL::do_intersect(segment2, s)) {
                        expr += xvars[edge(i, j)];
                        break;
                    }
                }
            }
            addLazy(expr >= 2);
        } catch (CGAL::Failure_exception &e) {
            std::cout << "CGAL Error during glue cut in callback" << std::endl;
            std::cout << e.what() << std::endl;
        } catch (...) {
            std::cout << "Error during glue cut in callback" << std::endl;
        }
    }

    void LazyCycle::add_tail_cut(const std::vector<Point> &cut, std::vector<std::size_t> indices) {
        try {
            std::vector<std::size_t> edgeIndices;
            for (int i = 0; i < indices.size() - 1; ++i) {
                for (int j = i + 1; j < indices.size(); ++j) {
                    edgeIndices.emplace_back(algutil::upper_no_diagonal::index_unordered(n, indices[i], indices[j]));
                }
            }
            std::sort(edgeIndices.begin(), edgeIndices.end());
            std::sort(indices.begin(), indices.end());

            std::vector<Segment> segments;
            for (int j, i = 0; i < cut.size() - 1; i = j) {
                j = i + 1;
                if (j < cut.size() - 1) {
                    while (j < cut.size() - 1 && CGAL::collinear(cut[i], cut[j], cut[j + 1])) j++;
                }
                segments.emplace_back(Segment{cut[i], cut[j]});
            }

            GRBLinExpr expr = 0;

            auto edges = boost::edges(instance.g);
            for (auto e = edges.first; e != edges.second; ++e) {
                auto i = boost::source(*e, instance.g);
                auto j = boost::target(*e, instance.g);
                auto idx = algutil::upper_no_diagonal::index_unordered(n, i, j);
                if (std::binary_search(edgeIndices.begin(), edgeIndices.end(), idx)) {
                    continue;
                } else if (std::binary_search(indices.begin(), indices.end(), i) ||
                           std::binary_search(indices.begin(), indices.end(), j)) {
                    expr += xvars[edge(i, j)];
                } else {
                    Segment segment2{points[i], points[j]};
                    for (auto &s : segments) {
                        if (CGAL::do_intersect(segment2, s)) {
                            expr += xvars[edge(i, j)];
                            break;
                        }
                    }
                }
            }
            addLazy(expr >= 1);
        } catch (CGAL::Failure_exception &e) {
            std::cout << "CGAL Error during tail cut in callback" << std::endl;
            std::cout << e.what() << std::endl;
        } catch (...) {
            std::cout << "Error during tail cut in callback" << std::endl;
        }
    }

    void LazyCycle::add_hih_cut(const std::vector<Point> &cut, std::vector<std::size_t> indices) {
        try {
            std::vector<std::size_t> edgeIndices;
            for (int i = 0; i < indices.size() - 1; ++i) {
                for (int j = i + 1; j < indices.size(); ++j) {
                    // all possible inner cycle edges
                    edgeIndices.emplace_back(algutil::upper_no_diagonal::index_unordered(n, indices[i], indices[j]));
                }
            }
            std::sort(edgeIndices.begin(), edgeIndices.end());
            std::sort(indices.begin(), indices.end());

            std::vector<Segment> segments;
            for (int j, i = 0; i < cut.size() - 1; i = j) {
                j = i + 1;
                if (j < cut.size() - 1) {
                    while (j < cut.size() - 1 && CGAL::collinear(cut[i], cut[j], cut[j + 1])) j++;
                }
                segments.emplace_back(Segment{cut[i], cut[j]});
            }

            GRBLinExpr expr = 0;

            auto edges = boost::edges(instance.g);
            for (auto e = edges.first; e != edges.second; ++e) {
                auto i = boost::source(*e, instance.g);
                auto j = boost::target(*e, instance.g);
                auto idx = algutil::upper_no_diagonal::index_unordered(n, i, j);
                if (std::binary_search(edgeIndices.begin(), edgeIndices.end(), idx)) {
                    continue;
                } else if (std::binary_search(indices.begin(), indices.end(), i) ||
                           std::binary_search(indices.begin(), indices.end(), j)) {
                    expr += xvars[edge(i, j)];
                } else {
                    Segment segment2{points[i], points[j]};
                    for (auto &s : segments) {
                        if (CGAL::do_intersect(segment2, s)) {
                            if (getSolution(xvars[edge(i, j)]) > 0.5) {
                                expr -= xvars[edge(i, j)];
                            } else {
                                expr += xvars[edge(i, j)];
                            }
                            break;
                        }
                    }
                }
            }
            addLazy(expr >= -1);
        } catch (CGAL::Failure_exception &e) {
            std::cout << "CGAL Error during HiH cut in callback" << std::endl;
            std::cout << e.what() << std::endl;
        } catch (...) {
            std::cout << "Error during HiH cut in callback" << std::endl;
        }
    }

    void LazyCycle::plot() {
        Graph g;
        for (auto &p : instance.points) {
            boost::add_vertex(p, g);
        }

        for (auto e = boost::edges(instance.g).first; e != boost::edges(instance.g).second; ++e) {
            auto i = boost::source(*e, instance.g);
            auto j = boost::target(*e, instance.g);
            auto var = getSolution(xvars[edge(i, j)]);
            if (var >= 0.5) {
                boost::add_edge(i, j, g);
            }
        }

        CGAL::gnuplot_output_2(g, "cb" + boost::lexical_cast<std::string>(cbCnt), nullptr, true);
    }

    void LazyCycle::plot_with_cut(const std::vector<Point> & cut) {
        Graph g;
        for (auto &p : instance.points) {
            boost::add_vertex(p, g);
        }

        for (auto e = boost::edges(instance.g).first; e != boost::edges(instance.g).second; ++e) {
            auto i = boost::source(*e, instance.g);
            auto j = boost::target(*e, instance.g);
            auto var = getSolution(xvars[edge(i, j)]);
            if (var >= 0.5) {
                boost::add_edge(i, j, g);
            }
        }

        std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>> vec;
        for (int j, i = 0; i < cut.size() - 1; i = j) {
            j = i + 1;
            if (j < cut.size() - 1) {
                while (j < cut.size() - 1 && CGAL::collinear(cut[i], cut[j], cut[j + 1])) j++;
            }
            std::pair<double,double> p1 = std::make_pair(boost::lexical_cast<double>(cut[i].x()), boost::lexical_cast<double>(cut[i].y()));
            std::pair<double,double> p2 = std::make_pair(boost::lexical_cast<double>(cut[j].x()), boost::lexical_cast<double>(cut[j].y()));
            vec.emplace_back(std::make_pair(p1, p2));
        }

        CGAL::gnuplot_output_2(g, "cb" + boost::lexical_cast<std::string>(cbCnt), nullptr, true, &vec);
    }

    Min_perimeter_polygon::Min_perimeter_polygon(Solver::Instance &_instance, Solver_type solver_type, std::vector<std::pair<std::size_t, std::size_t>> &ws_edges, bool lazy,
                                                 bool warm_start, int time_limit)
            : instance(_instance), solver_choice(solver_type), time_limit(time_limit) {
        std::string type = solver_type_str(solver_choice);
        solution = std::make_shared<Min_perimeter_polygon_solution>(instance, type);
        GRBEnv *env;

        try {
            env = new GRBEnv();
            GRBModel model = GRBModel(*env);
            if (time_limit > 0) {
                model.set("TimeLimit", boost::lexical_cast<std::string>(time_limit));
                solution->put_json_int("TimeLimit", time_limit);
            }

            defineModel(model);

            if (ws_edges.size() > 0) {
                for (auto &xvar : xvars) {
                    xvar.second.set(GRB_DoubleAttr_Start, 0.0);
                }
                for (auto &[p, q] : ws_edges) {
                    xvars[edge(p, q)].set(GRB_DoubleAttr_Start, 1.0);
                }
            } else if (warm_start) {
                json ws = jump_start_delaunay();
                solution->json_mpp["WarmStart"] = ws;
            }

            if (lazy) {
                solveLazy(model);
            } else {
                solveIncrementally(model);
            }
            if (model.get(GRB_IntAttr_Status) != GRB_INFEASIBLE && model.get(GRB_IntAttr_SolCount) > 0) {

                std::vector<Graph::edge_descriptor> solution_edges;

                auto edges = boost::edges(instance.g);
                for (auto e = edges.first; e != edges.second; ++e) {
                    auto i = boost::source(*e, instance.g);
                    auto j = boost::target(*e, instance.g);
                    auto var = xvars[edge(i, j)].get(GRB_DoubleAttr_X);
                    if (var >= 0.5) {
                        solution_edges.emplace_back(*e);
                    }
                }

                solution->put_edges(solution_edges);

                std::cout << "Model Runtime: " << model.get(GRB_DoubleAttr_Runtime) << std::endl;
            }


        } catch (GRBException &e) {
            std::cout << "Error during optimization" << std::endl;
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during optimization" << std::endl;
        }
        delete env;
    }

    void Min_perimeter_polygon::defineModel(GRBModel &model) {
        auto edges = boost::edges(instance.g);
        /*
         * initialize all x and y variables
         */
        for (auto e = edges.first; e != edges.second; ++e) {
            auto i = boost::source(*e, instance.g);
            auto j = boost::target(*e, instance.g);
            /*
             * x variables for all edges (i,j)
             */
            xvars[edge(i, j)] = model.addVar(0.0, 1.0,
                                             CGAL::sqrt(CGAL::squared_distance(instance.points[i], instance.points[j])),
                                             solver_choice == MPP ? GRB_BINARY : GRB_CONTINUOUS,
                                             "x_" + boost::lexical_cast<std::string>(i)
                                             + "_" + boost::lexical_cast<std::string>(j));
            xvars[edge(j, i)] = xvars[edge(i, j)];
        }
        /*
         * degree constraint
         */
        auto vs = boost::vertices(instance.g);
        for (auto v = vs.first; v != vs.second; ++v) {
            GRBLinExpr expr = 0;
            auto v_out_edges = boost::out_edges(*v, instance.g);
            for (auto e = v_out_edges.first; e != v_out_edges.second; ++e) {
                auto w = boost::target(*e, instance.g);
                expr += xvars[edge(*v, w)];
            }
            model.addConstr(expr == 2, "deg2_" + boost::lexical_cast<std::string>(*v));
        }
    }

    void Min_perimeter_polygon::solveLazy(GRBModel &model) {
        model.set(GRB_IntParam_LazyConstraints, 1);
        LazyCycle cb = LazyCycle(xvars, instance);
        model.setCallback(&cb);

        model.optimize();

        std::cout << "Lazy callback calls: " << cb.cbCnt << std::endl;
        std::cout << "Cycle constraints added: " << cb.cycleCutCnt << std::endl;
        std::cout << "Glue constraints added: " << cb.glueCutCnt << std::endl;
        std::cout << "Tail constraints added: " << cb.tailCutCnt << std::endl;
        std::cout << "HiH constraints added: " << cb.hihCutCnt << std::endl;


        solution->put_json_int("lazyCbCnt", cb.cbCnt);
        solution->put_json_int("lazyCycleCutCnt", cb.cycleCutCnt);
        solution->put_json_int("lazyGlueCutCnt", cb.glueCutCnt);
        solution->put_json_int("lazyTailCutCnt", cb.tailCutCnt);
        solution->put_json_int("lazyHiHCutCnt", cb.hihCutCnt);
        solution->put_model_solution(json::parse(model.getJSONSolution()));
    }

    void Min_perimeter_polygon::solveIncrementally(GRBModel &model) {

    }

    std::shared_ptr<Min_perimeter_polygon_solution> Min_perimeter_polygon::getSolution() {
        return solution;
    }

    json Min_perimeter_polygon::jump_start_delaunay() {

        Sparsification::EdgeSparsification dt_sparsification{instance.points};
        Sparsification::delaunay_triangulation(dt_sparsification);

        Solver::Instance dt_instance{dt_sparsification};
        dt_instance.instance_name = "dt_warm_start";
        dt_instance.pointset_name = instance.pointset_name;

        std::vector<std::pair<std::size_t, std::size_t>> ws_edges;
        Min_perimeter_polygon dt_mpp{dt_instance, MPP, ws_edges, true, false, time_limit};
        auto dt_mpp_sol = dt_mpp.getSolution();

        for (auto &xvar : xvars) {
            xvar.second.set(GRB_DoubleAttr_Start, 0.0);
        }

        for (auto &e : dt_mpp_sol->solution_edges) {
            auto s = boost::source(e, dt_instance.g);
            auto t = boost::target(e, dt_instance.g);
            xvars[edge(s, t)].set(GRB_DoubleAttr_Start, 1.0);
        }

        return dt_mpp_sol->json_mpp;
    }

    json Min_perimeter_polygon::jump_start_mwt() {

        Sparsification::EdgeSparsification mwt_sparsification{instance.points};
        Sparsification::minimum_weight_triangulation(mwt_sparsification);

        Solver::Instance mwt_instance{mwt_sparsification};
        mwt_instance.instance_name = "mwt_warm_start";
        mwt_instance.pointset_name = instance.pointset_name;

        std::vector<std::pair<std::size_t, std::size_t>> ws_edges;
        Min_perimeter_polygon mwt_mpp{mwt_instance, MPP, ws_edges, true, false, time_limit};
        auto mwt_mpp_sol = mwt_mpp.getSolution();

        for (auto &xvar : xvars) {
            xvar.second.set(GRB_DoubleAttr_Start, 0.0);
        }

        for (auto &e : mwt_mpp_sol->solution_edges) {
            auto s = boost::source(e, mwt_instance.g);
            auto t = boost::target(e, mwt_instance.g);
            xvars[edge(s, t)].set(GRB_DoubleAttr_Start, 1.0);
        }

        return mwt_mpp_sol->json_mpp;
    }

}