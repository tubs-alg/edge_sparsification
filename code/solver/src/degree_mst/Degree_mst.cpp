#include "degree_mst/Degree_mst.h"

namespace dmst {

    std::string solver_type_str(Solver_type type) {
        switch (type) {
            case DEGREE_MST:
                return "Degree_mst";
            default:
                return "";
        }
    }

    LazyDegreeMST::LazyDegreeMST(std::map<std::size_t, GRBVar> &xv, Solver::Instance &_instance) :
            xvars(xv), instance(_instance) {
        n = boost::num_vertices(instance.g);
        cbCnt = 0;
        secCnt = 0;
    }

    void LazyDegreeMST::callback() {
        try {
            if (where == GRB_CB_MIPSOL) {
                cbCnt++;

//                plot();
//                std::cout << "MIPSOL CB" << std::endl;

                Graph g{n};
                for (auto e = boost::edges(instance.g).first; e != boost::edges(instance.g).second; ++e) {
                    auto i = boost::source(*e, instance.g);
                    auto j = boost::target(*e, instance.g);
                    auto idx = algutil::upper_no_diagonal::index_unordered(n, i, j);
                    if (getSolution(xvars[idx]) >= 0.5) {
                        boost::add_edge(i, j, g);
                    }
                }

                if (find_violated_sec(g)) return;

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

    bool LazyDegreeMST::find_violated_sec(const Graph &g) {
        std::vector<int> component(n);
        int num = connected_components(g, &component[0]);
        std::vector<std::size_t> component_num_edges(num, 0);
        std::vector<std::size_t> component_num_vertices(num, 0);
        for (std::size_t i = 0; i < n; ++i) {
            component_num_vertices[component[i]]++;
            component_num_edges[component[i]] += boost::out_degree(i, g);
        }
        bool found_violated_sec = false;
        for (int i = 0; i < num; ++i) {
            if (component_num_edges[i] >= (2 * component_num_vertices[i])) {
                std::vector<std::size_t> ind;
                for (int j = 0; j < n; ++j) {
                    if (component[j] == i) {
                        ind.emplace_back(j);
                    }
                }
                add_subtour_constraint(ind);
                secCnt++;
                found_violated_sec = true;
            }
        }

        return found_violated_sec;
    }

    void LazyDegreeMST::add_subtour_constraint(const std::vector<std::size_t> &indices) {
        std::vector<bool> currentCycle = std::vector<bool>(n, false);
        auto len = indices.size();
        for (int i = 0; i < len; ++i) currentCycle[indices[i]] = true;
        std::unordered_set<std::size_t, boost::hash<std::size_t>> expr_edges;
        for (int i = 0; i < len; ++i) {
            for (auto e : boost::make_iterator_range(boost::out_edges(indices[i], instance.g))) {
                auto target = boost::target(e, instance.g);
                if (!currentCycle[target]) {
                    continue;
                } else {
                    expr_edges.insert(algutil::upper_no_diagonal::index_unordered(n, indices[i], target));
                }
            }
        }
        GRBLinExpr expr = 0;
        for (auto e : expr_edges) {
            expr += xvars[e];
        }
        addLazy(expr <= len - 1);
    }

    void LazyDegreeMST::plot() {
        Graph g;
        for (auto &p : instance.points) {
            boost::add_vertex(p, g);
        }

        std::vector<Graph::edge_descriptor> solution_edges;

        for (auto e = boost::edges(instance.g).first; e != boost::edges(instance.g).second; ++e) {
            auto i = boost::source(*e, instance.g);
            auto j = boost::target(*e, instance.g);
            auto idx = algutil::upper_no_diagonal::index_unordered(n, i, j);
            auto var = getSolution(xvars[idx]);
            if (var >= 0.5) {
                solution_edges.emplace_back(*e);
                boost::add_edge(i, j, g);
            }
        }

        CGAL::gnuplot_output_2(g, "cb" + boost::lexical_cast<std::string>(cbCnt), &solution_edges, true);
    }

    Degree_mst::Degree_mst(Solver::Instance &_instance, Solver_type solver_type, int _degree, bool warm_start, int time_limit, bool output)
            : instance(_instance), solver_choice(solver_type), degree(_degree), time_limit(time_limit), output(output) {
        std::string type = solver_type_str(solver_choice);
        solution = std::make_shared<Degree_mst_solution>(instance, type);
        n = boost::num_vertices(instance.g);
        GRBEnv *env;

        try {
            env = new GRBEnv();
            GRBModel model = GRBModel(*env);
            if (time_limit > 0) {
                model.set("TimeLimit", boost::lexical_cast<std::string>(time_limit));
                solution->put_json_int("TimeLimit", time_limit);
            }

            if (!output) {
                model.set("LogToConsole", "0");
            }

            defineModel(model);

            if (warm_start) {
                json ws = jump_start_mst();
                solution->json_dmst["WarmStart"] = ws;
            }

            solveLazy(model);

            if (model.get(GRB_IntAttr_Status) != GRB_INFEASIBLE && model.get(GRB_IntAttr_SolCount) > 0) {

                std::vector<Graph::edge_descriptor> solution_edges;
                std::vector<std::pair<std::size_t, std::size_t>> solution_cones;

                auto edges = boost::edges(instance.g);
                for (auto e = edges.first; e != edges.second; ++e) {
                    auto i = boost::source(*e, instance.g);
                    auto j = boost::target(*e, instance.g);
                    auto idx = algutil::upper_no_diagonal::index_unordered(n, i, j);
                    auto var = xvars[idx].get(GRB_DoubleAttr_X);
                    if (var >= 0.5) {
                        solution_edges.emplace_back(*e);
                    }
                }

                solution->put_edges(solution_edges);

                if (output) {
                    std::cout << "Model Runtime: " << model.get(GRB_DoubleAttr_Runtime) << std::endl;
                }
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

    void Degree_mst::defineModel(GRBModel &model) {
        auto edges = boost::edges(instance.g);
        /*
         * initialize all x variables
         * define n-1 edges constraint
         */
        GRBLinExpr expr = 0;
        for (auto e = edges.first; e != edges.second; ++e) {
            auto i = boost::source(*e, instance.g);
            auto j = boost::target(*e, instance.g);
            /*
             * x variables for all edges (i,j)
             */
            auto idx = algutil::upper_no_diagonal::index_unordered(n, i, j);
            auto xvar = model.addVar(0.0, 1.0,
                                     CGAL::sqrt(CGAL::squared_distance(instance.points[i], instance.points[j])),
                                     GRB_BINARY,
                                     "x_" + boost::lexical_cast<std::string>(i)
                                     + "_" + boost::lexical_cast<std::string>(j));
            xvars[idx] = xvar;
            expr += xvars[idx];
        }
        /*
         * n-1 edges constraint
         */
        model.addConstr(expr == n - 1, "n-1_constraint");

        /*
         * degree constraint for all vertices
         */
        for (auto v : boost::make_iterator_range(boost::vertices(instance.g))) {
            expr = 0;
            for (auto e : boost::make_iterator_range(boost::out_edges(v, instance.g))) {
                auto target = boost::target(e, instance.g);
                auto idx = algutil::upper_no_diagonal::index_unordered(n, v, target);
                expr += xvars[idx];
            }
            model.addConstr(expr <= degree, "degree_" + boost::lexical_cast<std::string>(v));
            model.addConstr(expr >= 1, "degree1_" + boost::lexical_cast<std::string>(v));
        }

    }

    void Degree_mst::solveLazy(GRBModel &model) {
        model.set(GRB_IntParam_LazyConstraints, 1);
        LazyDegreeMST cb = LazyDegreeMST(xvars, instance);
        model.setCallback(&cb);

        model.optimize();

        objVal = model.getObjective().getValue();

        if (output) {
            std::cout << "Lazy callback calls: " << cb.cbCnt << std::endl;
            std::cout << "SEC count: " << cb.secCnt << std::endl;
        }

        solution->put_json_size_t("lazyCbCnt", cb.cbCnt);
        solution->put_json_size_t("secCnt", cb.secCnt);
        solution->put_model_solution(json::parse(model.getJSONSolution()));
    }

    std::shared_ptr<Degree_mst_solution> Degree_mst::getSolution() {
        return solution;
    }

    json Degree_mst::jump_start_delaunay() {

        Solver::Instance dt_instance{};
        dt_instance.instance_name = "dt_warm_start";
        dt_instance.pointset_name = instance.pointset_name;
        std::map<Point_2, std::size_t> point_map;
        int i = 0;
        for (auto &p : instance.points) {
            boost::add_vertex(p, dt_instance.g);
            dt_instance.points.emplace_back(p);
            point_map[p] = i++;
        }

        DelaunayTriangulation dt = DelaunayTriangulation{dt_instance.points.begin(), dt_instance.points.end()};

        for (auto it = dt.finite_edges_begin(); it != dt.finite_edges_end(); ++it) {
            Segment_2 seg = dt.segment(it);
            auto s = point_map[seg.source()];
            auto t = point_map[seg.target()];
            boost::add_edge(s, t, dt_instance.g);
        }

        Degree_mst dt_dmst{dt_instance, DEGREE_MST, degree, false, time_limit};
        auto dt_dmst_sol = dt_dmst.getSolution();

        for (auto &xvar : xvars) {
            xvar.second.set(GRB_DoubleAttr_Start, 0.0);
        }

        for (auto &e : dt_dmst_sol->solution_edges) {
            auto s = boost::source(e, dt_instance.g);
            auto t = boost::target(e, dt_instance.g);
            auto idx = algutil::upper_no_diagonal::index_unordered(n, s, t);
            xvars[idx].set(GRB_DoubleAttr_Start, 1.0);
        }

        return dt_dmst_sol->json_dmst;
    }

    json Degree_mst::jump_start_mst() {
        for (auto e : boost::make_iterator_range(boost::edges(instance.g))) {
            auto s = boost::source(e, instance.g);
            auto t = boost::target(e, instance.g);
            double distance = CGAL::sqrt(CGAL::squared_distance(instance.points[s], instance.points[t]));
            boost::put(boost::edge_weight_t(), instance.g, e, distance);
        }

        Graph mst_g{};
        for (auto &p : instance.points) {
            boost::add_vertex(p, mst_g);
        }

        std::vector<edge_descriptor> mst_edges;
        boost::kruskal_minimum_spanning_tree(instance.g, std::back_inserter(mst_edges));
        for (auto &e : mst_edges) {
            boost::add_edge(boost::source(e, instance.g), boost::target(e, instance.g), boost::get(boost::edge_weight_t(), instance.g, e), mst_g);
        }

        std::vector<edge_descriptor> violating_edges;
        for (auto e : boost::make_iterator_range(boost::edges(mst_g))) {
            auto s = boost::source(e, mst_g);
            auto t = boost::target(e, mst_g);
            if (boost::degree(s, mst_g) > degree && boost::degree(t, mst_g) > degree) {
                violating_edges.emplace_back(e);
            }
        }
        for (auto e : violating_edges) {
            boost::remove_edge(e, mst_g);
        }
        for (auto v : boost::make_iterator_range(boost::vertices(mst_g))) {
            while (boost::degree(v, mst_g) > degree) {
                double max_length_value = 0;
                edge_descriptor max_length_edge;
                for (auto e : boost::make_iterator_range(boost::incident_edges(v, mst_g))) {
                    double len = boost::get(boost::edge_weight_t(), mst_g, e);
                    if (len > max_length_value) {
                        max_length_value = len;
                        max_length_edge = e;
                    }
                }
                boost::remove_edge(max_length_edge, mst_g);
            }
        }

        std::size_t num;
        do {
            std::vector<int> component(n);
            num = connected_components(mst_g, &component[0]);
            if (num == 1) break;
            std::vector<std::vector<std::size_t>> component_vertices(num, std::vector<std::size_t>());
            for (std::size_t i = 0; i < n; ++i) {
                if (boost::degree(i, mst_g) < degree) {
                    component_vertices[component[i]].emplace_back(i);
                }
            }
            double min_length_value = std::numeric_limits<double>::max();
            std::pair<std::size_t, std::size_t> min_length_edge;
            for (std::size_t i = 0; i < num; ++i) {
                for (auto v : component_vertices[i]) {
                    if (boost::degree(v, mst_g) < degree) {
                        for (auto e : boost::make_iterator_range(boost::incident_edges(v, instance.g))) {
                            auto t = boost::target(e, instance.g);
                            if (component[t] > i
                                && !boost::edge(v, t, mst_g).second
                                && component[t] != i
                                && boost::degree(t, mst_g) < degree) {
                                auto len = boost::get(boost::edge_weight_t(), instance.g, e);
                                if (len < min_length_value) {
                                    min_length_value = len;
                                    min_length_edge = std::make_pair(v, t);
                                }
                            }
                        }
                    }
                }
            }
            if (min_length_value < std::numeric_limits<double>::max()) {
                boost::add_edge(min_length_edge.first, min_length_edge.second, min_length_value, mst_g);
            } else {
                break;
            }
        } while (num > 1);

        for (auto &xvar : xvars) {
            xvar.second.set(GRB_DoubleAttr_Start, 0.0);
        }

        double mst_weight = 0;
        json json_edges;
        for (auto e : boost::make_iterator_range(boost::edges(mst_g))) {
            mst_weight += boost::get(boost::edge_weight_t(), mst_g, e);
            auto s = boost::source(e, mst_g);
            auto t = boost::target(e, mst_g);
            json_edges.emplace_back(json{s, t});
            auto idx = algutil::upper_no_diagonal::index_unordered(n, s, t);
            xvars[idx].set(GRB_DoubleAttr_Start, 1.0);
        }

        json mst_json;
        mst_json["weight"] = mst_weight;
        mst_json["edges"] = json_edges;

        return mst_json;

    }

}