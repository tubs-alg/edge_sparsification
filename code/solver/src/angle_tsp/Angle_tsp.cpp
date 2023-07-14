#include "angle_tsp/Angle_tsp.h"

namespace angle_tsp {

    std::string solver_type_str(Solver_type type) {
        switch (type) {
            case ANGLE:
                return "Angle_tsp";
            case ANGLE_LB:
                return "Angle_tsp_lb";
            case ANGLE_DISTANCE:
                return "Angle_distance_tsp";
            default:
                return "Angle_distance_tsp_lb";
        }
    }

    double turning_angle_ab(Vector_2 &a, Vector_2 &b) {
        if (CGAL::orientation(a, b) == CGAL::COLLINEAR) {
            if (a.direction() == b.direction()) {
                return 0.0;
            } else {
                return 3.141592653589793;
            }
        } else {
            double prod = a * b;
            double len_prod = (CGAL::sqrt(a.squared_length()) * CGAL::sqrt(b.squared_length()));
            double div = prod / len_prod;
            if (div < -1) {
                div = -1;
            } else if (div > 1) {
                div = 1;
            }
//            double angle = std::acos(a * b / (CGAL::sqrt(a.squared_length()) * CGAL::sqrt(b.squared_length())));
            double angle = std::acos(div);
            return angle;
        }
    }

    double angle_cost(Vector_2 &a, Vector_2 &b) {
        double angle = turning_angle_ab(a, b);
        angle = angle * 1000;
        angle = std::ceil(angle * MULTIPLIER) / MULTIPLIER;
        return angle;
    }

    double angle_distance_cost(Vector_2 &a, Vector_2 &b, std::size_t rho, std::size_t lambda1, std::size_t lambda2) {
        double angle = turning_angle_ab(a, b);
        double weighted_distance = (CGAL::sqrt(a.squared_length()) + CGAL::sqrt(b.squared_length())) / 2;
        angle = lambda1 * rho * angle;
        weighted_distance = lambda2 * weighted_distance;
        double ans = angle + weighted_distance;
        ans = std::ceil(ans * MULTIPLIER) / MULTIPLIER;
        return ans;
    }

    LazySubtour::LazySubtour(std::map<edge, GRBVar> &xv, Graph &graph, std::size_t num_nodes, bool all_subt) {
        xvars = xv;
        g = graph;
        n = num_nodes;
        all_subtours = all_subt;
        cbCnt = 0;
        lazyConstrCnt = 0;
    }

    void LazySubtour::callback() {
        try {
            if (where == GRB_CB_MIPSOL) {
                cbCnt++;
                std::vector<bool> visited = std::vector<bool>(n, false);
                std::vector<std::size_t> tour = std::vector<std::size_t>(n);
                int len;
                std::size_t start_node;

                bool subtour = true;
                while (subtour) {
                    subtour = false;
                    for (std::size_t i = 0; i < n; ++i) {
                        if (!visited[i]) {
                            subtour = true;
                            start_node = i;
                            break;
                        }
                    }
                    if (subtour) {
                        len = find_subtour(visited, tour, start_node);
                        if (len < n) {
                            add_constraint_type_1(tour, len);
                            lazyConstrCnt++;
                        }
                    }
                    if (!all_subtours) break;
                }
            }
        } catch (GRBException &e) {
            std::cout << "Error during callback" << std::endl;
            std::cout << "Error number: " << e.getErrorCode() << std::endl;
            std::cout << e.getMessage() << std::endl;
        } catch (...) {
            std::cout << "Error during callback" << std::endl;
        }
    }

    void LazySubtour::add_constraint_type_1(const std::vector<std::size_t> &tour, int len) {
        std::vector<bool> currentTour = std::vector<bool>(n, false);
        for (int i = 0; i < len; ++i) currentTour[tour[i]] = true;
        std::unordered_set<edge, boost::hash<edge>> expr_edges;
        for (int i = 0; i < len; ++i) {
            for (auto e : boost::make_iterator_range(boost::out_edges(tour[i], g))) {
                auto target = boost::target(e, g);
                if (!currentTour[target]) {
                    continue;
                } else {
                    if (tour[i] < target) {
                        expr_edges.insert(edge(tour[i], target));
                    } else {
                        expr_edges.insert(edge(target, tour[i]));
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

    void LazySubtour::add_constraint_type_2(const std::vector<std::size_t> &tour, int len) {
        std::vector<bool> currentTour = std::vector<bool>(n, false);
        for (int i = 0; i < len; ++i) currentTour[tour[i]] = true;
        GRBLinExpr expr = 0;
        for (int i = 0; i < len; ++i) {
            for (auto e : boost::make_iterator_range(boost::out_edges(tour[i], g))) {
                auto target = boost::target(e, g);
                if (currentTour[target]) {
                    continue;
                } else {
                    expr += xvars[edge(tour[i], target)];
                }
            }
        }
        addLazy(expr >= 2);
    }

    int LazySubtour::find_subtour(std::vector<bool> &visited, std::vector<std::size_t> &tour, std::size_t start) {
        int len = 0;
        std::size_t vertex = start;
        while (!visited[vertex]) {
            visited[vertex] = true;
            tour[len] = vertex;
            len++;
            auto edges = boost::out_edges(vertex, g);
            for (auto e = edges.first; e != edges.second; ++e) {
                auto target = boost::target(*e, g);
                if (visited[target]) {
                    continue;
                }
                if (getSolution(xvars[edge(vertex, target)]) >= 0.5) {
                    vertex = target;
                    break;
                }
            }
        }
        return len;
    }

    void LazySubtour::plot_integral_cb_solution() {
        std::vector<Graph::edge_descriptor> tourEdges;
        for (auto e : boost::make_iterator_range(boost::edges(g))) {
            auto source = boost::source(e, g);
            auto target = boost::target(e, g);
            if (getSolution(xvars[edge(source, target)]) >= 0.5) {
                tourEdges.emplace_back(e);
            }
        }

        CGAL::gnuplot_output_2(g, "ANG_CB_" + boost::lexical_cast<std::string>(cbCnt), &tourEdges, true);
    }

    Angle_tsp::Angle_tsp(Solver::Instance &_instance, Solver_type solver_type, bool lazy, bool all_subtours,
                         int time_limit) : instance(_instance), solver_choice(solver_type) {
        std::string type = solver_type_str(solver_choice);
        solution = std::make_shared<Angle_tsp_solution>(instance, type);
        GRBEnv *env;

        try {
            env = new GRBEnv();
            GRBModel model = GRBModel(*env);
            if (time_limit > 0) {
                model.set("TimeLimit", boost::lexical_cast<std::string>(time_limit));
                solution->put_json_int("TimeLimit", time_limit);
            }

            defineModel(model);

            if (solver_choice == ANGLE || solver_choice == ANGLE_DISTANCE) {
                if (lazy) {
                    solveLazy(model, all_subtours);
                } else {
                    solveIncrementally(model, all_subtours);
                }
                if (model.get(GRB_IntAttr_Status) != GRB_INFEASIBLE && model.get(GRB_IntAttr_SolCount) > 0) {

                    std::vector<Graph::edge_descriptor> tour_edges;

                    auto edges = boost::edges(instance.g);
                    for (auto e = edges.first; e != edges.second; ++e) {
                        auto i = boost::source(*e, instance.g);
                        auto j = boost::target(*e, instance.g);
                        auto var = xvars[edge(i, j)].get(GRB_DoubleAttr_X);
                        if (var >= 0.5) {
                            tour_edges.emplace_back(*e);
                        }
                    }

                    solution->put_tour_edges(tour_edges);
                }
            } else {
                solveLb(model);
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

    void Angle_tsp::defineModel(GRBModel &model) {
        auto edges = boost::edges(instance.g);
        /*
         * initialize all x and y variables
         */for (auto e = edges.first; e != edges.second; ++e) {
            auto i = boost::source(*e, instance.g);
            auto j = boost::target(*e, instance.g);
            /*
             * x variables for all edges (i,j)
             */
            xvars[edge(i, j)] = model.addVar(0.0, 1.0, 0,
                                             solver_choice == ANGLE || solver_choice == ANGLE_DISTANCE ? GRB_BINARY
                                                                                                       : GRB_CONTINUOUS,
                                             "x_" + boost::lexical_cast<std::string>(i)
                                             + "_" + boost::lexical_cast<std::string>(j));
            xvars[edge(j, i)] = xvars[edge(i, j)];
            /*
             * y variables for all turns (j,i,k)
             */
            GRBLinExpr expr = 0; // constraint to link x and y variables
            auto i_out_edges = boost::out_edges(i, instance.g);
            for (auto i_out = i_out_edges.first; i_out != i_out_edges.second; ++i_out) {
                auto k = boost::target(*i_out, instance.g);
                if (k != j) {
                    if (!yvars.count(triple{j, i, k})) {
                        Vector_2 a{instance.g[j], instance.g[i]};
                        Vector_2 b{instance.g[i], instance.g[k]};
                        yvars[triple{j, i, k}] = model.addVar(0.0, 1.0,
                                                              solver_choice == ANGLE || solver_choice == ANGLE_LB
                                                              ? angle_cost(a, b) : angle_distance_cost(a, b, 40),
                                                              solver_choice == ANGLE || solver_choice == ANGLE_DISTANCE
                                                              ? GRB_BINARY : GRB_CONTINUOUS,
                                                              "y_" + boost::lexical_cast<std::string>(j)
                                                              + "_" + boost::lexical_cast<std::string>(i)
                                                              + "_" + boost::lexical_cast<std::string>(k));
                        yvars[triple{k, i, j}] = yvars[triple{j, i, k}];
                    }
                    expr += yvars[triple{j, i, k}]; // add variable y_jik to constraint
                }
            }
            if (expr.size() > 0) {
                model.addConstr(expr == xvars[edge(j, i)], "link_" + boost::lexical_cast<std::string>(j) + "_" +
                                                           boost::lexical_cast<std::string>(i));
            }
            /*
             * y variables for all turns (i,j,k)
             */
            expr.clear();
            auto j_out_edges = boost::out_edges(j, instance.g);
            for (auto j_out = j_out_edges.first; j_out != j_out_edges.second; ++j_out) {
                auto k = boost::target(*j_out, instance.g);
                if (k != i) {
                    if (!yvars.count(triple{i, j, k})) {
                        Vector_2 a{instance.g[i], instance.g[j]};
                        Vector_2 b{instance.g[j], instance.g[k]};
                        yvars[triple{i, j, k}] = model.addVar(0.0, 1.0,
                                                              solver_choice == ANGLE || solver_choice == ANGLE_LB
                                                              ? angle_cost(a, b) : angle_distance_cost(a, b, 40),
                                                              solver_choice == ANGLE || solver_choice == ANGLE_DISTANCE
                                                              ? GRB_BINARY : GRB_CONTINUOUS,
                                                              "y_" + boost::lexical_cast<std::string>(i) + "_" +
                                                              boost::lexical_cast<std::string>(j) + "_" +
                                                              boost::lexical_cast<std::string>(k));
                        yvars[triple{k, j, i}] = yvars[triple{i, j, k}];
                    }
                    expr += yvars[triple{i, j, k}]; // add variable y_ijk to constraint
                }
            }
            if (expr.size() > 0) {
                model.addConstr(expr == xvars[edge(i, j)], "link_" + boost::lexical_cast<std::string>(i) + "_" +
                                                           boost::lexical_cast<std::string>(j));
            }
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

    void Angle_tsp::solveLazy(GRBModel &model, bool all_subtours) {
        model.set(GRB_IntParam_LazyConstraints, 1);
        LazySubtour cb = LazySubtour(xvars, instance.g, boost::num_vertices(instance.g), all_subtours);
        model.setCallback(&cb);

        model.optimize();

        std::cout << "Lazy callback calls: " << cb.cbCnt << std::endl;
        std::cout << "Lazy constraints added: " << cb.lazyConstrCnt << std::endl;

        solution->put_json_int("lazyCbCnt", cb.cbCnt);
        solution->put_json_int("lazyConstrCnt", cb.lazyConstrCnt);
        solution->put_model_solution(json::parse(model.getJSONSolution()));
    }

    std::shared_ptr<Angle_tsp_solution> Angle_tsp::getSolution() {
        return solution;
    }

    void Angle_tsp::solveIncrementally(GRBModel &model, bool all_subtours) {
        bool solved = false;
        int optimizationCnt = 0;
        int constrCnt = 0;
        double runtime = 0;

        while (!solved) {
            optimizationCnt++;
            model.optimize();
            runtime += model.get(GRB_DoubleAttr_Runtime);

            std::vector<bool> visited = std::vector<bool>(boost::num_vertices(instance.g), false);
            std::vector<std::size_t> tour = std::vector<std::size_t>(boost::num_vertices(instance.g));
            int len;
            std::size_t start_node;

            bool subtour = true;
            while (subtour) {
                subtour = false;
                for (std::size_t i = 0; i < boost::num_vertices(instance.g); ++i) {
                    if (!visited[i]) {
                        subtour = true;
                        start_node = i;
                        break;
                    }
                }
                if (subtour) {
                    std::size_t vertex = start_node;
                    len = 0;
                    while (!visited[vertex]) {
                        visited[vertex] = true;
                        tour[len] = vertex;
                        len++;
                        auto edges = boost::out_edges(vertex, instance.g);
                        for (auto e = edges.first; e != edges.second; ++e) {
                            auto target = boost::target(*e, instance.g);
                            if (visited[target]) {
                                continue;
                            }
                            if (xvars[edge(vertex, target)].get(GRB_DoubleAttr_X) > 0.5) {
                                vertex = target;
                                break;
                            }
                        }
                    }
                    if (len < boost::num_vertices(instance.g)) {
                        std::vector<bool> currentTour = std::vector<bool>(boost::num_vertices(instance.g), false);
                        for (int i = 0; i < len; ++i) currentTour[tour[i]] = true;
                        std::unordered_set<edge, boost::hash<edge>> expr_edges;
                        for (int i = 0; i < len; ++i) {
                            for (auto e : boost::make_iterator_range(boost::out_edges(tour[i], instance.g))) {
                                auto target = boost::target(e, instance.g);
                                if (!currentTour[target]) {
                                    continue;
                                } else {
                                    if (tour[i] < target) {
                                        expr_edges.insert(edge(tour[i], target));
                                    } else {
                                        expr_edges.insert(edge(target, tour[i]));
                                    }
                                }
                            }
                        }
                        GRBLinExpr expr = 0;
                        for (auto e : expr_edges) {
                            expr += xvars[e];
                        }
                        constrCnt++;
                        model.addConstr(expr <= len - 1);
                    }
                }
                if (!all_subtours) break;
            }
            if (len == boost::num_vertices(instance.g)) {
                solved = true;
            }
        }

        std::cout << "Optimization calls: " << optimizationCnt << std::endl;
        std::cout << "Constraints added: " << constrCnt << std::endl;

        solution->put_json_int("optimizationCnt", optimizationCnt);
        solution->put_json_int("constrCnt", constrCnt);
        solution->put_model_solution(json::parse(model.getJSONSolution()));
        solution->put_json_double("Runtime", runtime); // sum of all optimization calls
    }

    void Angle_tsp::solveLb(GRBModel &model) {

        bool subtourViolation = true;

        std::cout << std::setprecision(20);

        int optimizationCnt = 0;
        int minCutCnt = 0;
        double runtime = 0;
        auto edges = boost::edges(instance.g);

        while (subtourViolation) {
            subtourViolation = false;
            optimizationCnt++;
            model.optimize();
            runtime += model.get(GRB_DoubleAttr_Runtime);

            for (auto e = edges.first; e != edges.second; ++e) {
                auto i = boost::source(*e, instance.g);
                auto j = boost::target(*e, instance.g);
                double var = xvars[edge(i, j)].get(GRB_DoubleAttr_X);
                boost::put(boost::edge_weight_t(), instance.g, *e, var);
            }

            BOOST_AUTO(parities,
                       boost::make_one_bit_color_map(num_vertices(instance.g), get(boost::vertex_index, instance.g)));
            double w = boost::stoer_wagner_min_cut(instance.g, get(boost::edge_weight, instance.g),
                                                   boost::parity_map(parities));

            while (w < 2 - 1e-4) {
                std::cout << "MIN-CUT = " << w << std::endl;
                minCutCnt++;

                subtourViolation = true;

                GRBLinExpr expr = 0;
                int len = 0;
                for (int i = 0; i < num_vertices(instance.g); ++i) {
                    if (get(parities, i)) {
                        len++;
                        for (auto e : boost::make_iterator_range(boost::out_edges(i, instance.g))) {
                            auto target = boost::target(e, instance.g);
                            if (get(parities, target)) {
                                continue;
                            } else {
                                expr += xvars[edge(i, target)];
                                boost::put(boost::edge_weight_t(), instance.g, e, 2);
                            }
                        }
                    }
                }

                model.addConstr(expr >= 2);

                w = boost::stoer_wagner_min_cut(instance.g, get(boost::edge_weight, instance.g),
                                                boost::parity_map(parities));

            }
        }

        solution->put_model_solution(json::parse(model.getJSONSolution()));
        solution->put_json_int("optimizationCnt", optimizationCnt);
        solution->put_json_int("minCutCnt", minCutCnt);
        solution->put_json_double("Runtime", runtime);
    }

}