#include "alpha_mst/Alpha_mst.h"

namespace amst {

    std::string solver_type_str(Solver_type type) {
        switch (type) {
            case ALPHA_MST:
                return "Alpha_mst";
            default:
                return "";
        }
    }

    double directed_angle_k_i_j(Direction_2 &j, Direction_2 &k) {
        auto vec_j = j.to_vector();
        auto vec_k = k.to_vector();
        double angle = std::atan2(vec_k.y(), vec_k.x()) - std::atan2(vec_j.y(), vec_j.x());
        if (angle < 0) {
            angle += 2 * M_PI;
        }
        return angle;
    }

    LazyAlphaMST::LazyAlphaMST(std::map<std::size_t, GRBVar> &xv, Solver::Instance &_instance) :
            xvars(xv), instance(_instance) {
        n = boost::num_vertices(instance.g);
        cbCnt = 0;
        secCnt = 0;
    }

    void LazyAlphaMST::callback() {
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

    bool LazyAlphaMST::find_violated_sec(const Graph &g) {
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

    void LazyAlphaMST::add_subtour_constraint(const std::vector<std::size_t> &indices) {
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

    void LazyAlphaMST::plot() {
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

    Alpha_mst::Alpha_mst(Solver::Instance &_instance, Solver_type solver_type, double _alpha, int time_limit)
            : instance(_instance), solver_choice(solver_type), alpha(_alpha), time_limit(time_limit) {
        std::string type = solver_type_str(solver_choice);
        solution = std::make_shared<Alpha_mst_solution>(instance, type);
        solution->put_json_double("alpha", alpha);
        n = boost::num_vertices(instance.g);
        GRBEnv *env;

        try {
            env = new GRBEnv();
            GRBModel model = GRBModel(*env);
            if (time_limit > 0) {
                model.set("TimeLimit", boost::lexical_cast<std::string>(time_limit));
                solution->put_json_int("TimeLimit", time_limit);
            }

            init_angles();

            defineModel(model);

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

                for (const auto &[e, var] : yvars) {
                    if (var.get(GRB_DoubleAttr_X) >= 0.5) {
                        solution_cones.emplace_back(e);
                    }
                }

                solution->put_edges(solution_edges);
                solution->put_cones(solution_cones);

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

    void Alpha_mst::init_angles() {

        angle_0_i_j = std::vector<std::map<std::size_t, double>>(n);

        Direction_2 dir_0{1, 0};
        for (auto v : boost::make_iterator_range(boost::vertices(instance.g))) {
            for (auto e : boost::make_iterator_range(boost::out_edges(v, instance.g))) {
                auto target = boost::target(e, instance.g);
                Direction_2 dir_target = Vector_2{instance.points[v], instance.points[target]}.direction();
                angle_0_i_j[v][target] = directed_angle_k_i_j(dir_0, dir_target);
            }
        }

    }

    void Alpha_mst::defineModel(GRBModel &model) {
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
            /*
             * y variables for angle position
             */
            auto yvar_ij = model.addVar(0.0, 1.0, 0, GRB_CONTINUOUS, "y_" + boost::lexical_cast<std::string>(i)
                                                                     + "_" + boost::lexical_cast<std::string>(j));
            auto yvar_ji = model.addVar(0.0, 1.0, 0, GRB_CONTINUOUS, "y_" + boost::lexical_cast<std::string>(j)
                                                                     + "_" + boost::lexical_cast<std::string>(i));
            yvars[edge(i, j)] = yvar_ij;
            yvars[edge(j, i)] = yvar_ji;
            model.addConstr(yvar_ij <= xvar);
            model.addConstr(yvar_ji <= xvar);
        }
        /*
         * n-1 edges constraint
         */
        model.addConstr(expr == n - 1, "n-1_constraint");

        /*
         * f.a. vertices: sum of all associated y vars has to be equal to 1
         */
        for (auto i : boost::make_iterator_range(boost::vertices(instance.g))) {
            expr = 0;
            for (auto j : boost::make_iterator_range(boost::adjacent_vertices(i, instance.g))) {
                expr += yvars[edge(i, j)];
            }
            model.addConstr(expr == 1, "y_sum-" + boost::lexical_cast<std::string>(i));
        }

        /*
         * x_e is less than the sum of all y_ij (y_ji) vars that cover x_e within radius alpha
         */
        for (auto i : boost::make_iterator_range(boost::vertices(instance.g))) {
            for (auto j : boost::make_iterator_range(boost::adjacent_vertices(i, instance.g))) {
                expr = 0;
                for (auto k : boost::make_iterator_range(boost::adjacent_vertices(i, instance.g))) {
                    auto diff = angle_0_i_j[i].at(j) - angle_0_i_j[i].at(k);
                    if (diff < 0) diff += 2 * M_PI;
                    if (diff <= alpha) {
                        expr += yvars[edge(i, k)];
                    }
                }
                auto idx = algutil::upper_no_diagonal::index_unordered(n, i, j);
                model.addConstr(xvars[idx] <= expr, "x_" + boost::lexical_cast<std::string>(i) + "_"
                                                    + boost::lexical_cast<std::string>(j) + "_covered");
            }
        }
    }

    void Alpha_mst::solveLazy(GRBModel &model) {
        model.set(GRB_IntParam_LazyConstraints, 1);
        LazyAlphaMST cb = LazyAlphaMST(xvars, instance);
        model.setCallback(&cb);

        model.optimize();

        std::cout << "Lazy callback calls: " << cb.cbCnt << std::endl;
        std::cout << "SEC count: " << cb.secCnt << std::endl;

        solution->put_json_size_t("lazyCbCnt", cb.cbCnt);
        solution->put_json_size_t("secCnt", cb.secCnt);
        solution->put_model_solution(json::parse(model.getJSONSolution()));
    }

    std::shared_ptr<Alpha_mst_solution> Alpha_mst::getSolution() {
        return solution;
    }

}