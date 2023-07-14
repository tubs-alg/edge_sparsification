#ifndef MWT_BASIC_MWT_SOLVER_HPP
#define MWT_BASIC_MWT_SOLVER_HPP

#include <algcplex/cplex.hpp>
#include <thread>
#include <mutex>

#include <iostream>
#include <vector>
#include <tuple>
#include <functional>

#include "empty_triangle.hpp"
#include "../utils/vector_utils.hpp"
#include "../utils/pythonic_utils.hpp"
#include "../CGAL/Lmt_halfedge.h"


template<typename Traits>
class minimum_weight_triangulation_ip_solver {
public:
    using Edge = CGAL::Lmt_halfedge<Traits> *;

    using triangle_container = std::vector<MWT::EmptyTriangle<Traits>>;
    using edge_container = std::vector<Edge>;

    struct result_type {
        triangle_container triangles;
        double value;
        bool solved;
    };

    explicit minimum_weight_triangulation_ip_solver(triangle_container &triangles,
                                                    std::set<Edge> &edge_set,
                                                    bool cpx_logging = true,
                                                    unsigned int triangle_generation_time=0) :
            triangles(triangles),
            edges(edge_set.begin(), edge_set.end()),
            n(triangles.size()),
            edge_count(edges.size()),
            env(),
            model(env),
            cplex(model),
            vars(env, n),
            cpx_logging(cpx_logging),
            callbacks(env, *this),
            triangle_generation_time(triangle_generation_time),
            construction_time(0),
            lazy_constrained_callback_time(0){

        cplex.setParam(IloCplex::Param::Threads, std::thread::hardware_concurrency());
        cplex.setParam(IloCplex::ParallelMode, IloCplex::Opportunistic);

        auto start = std::chrono::high_resolution_clock::now();

        //callbacks = Callbacks(env, *this);
        callbacks.set_callback(&minimum_weight_triangulation_ip_solver::lazy_constraint_callback);
        callbacks.use(cplex); // actually add all callbacks registered with M_callbacks to CPLEX

        if (!cpx_logging) {
            cplex.setOut(env.getNullStream());
        }

        IloNumExpr objective_expr(env);

        struct TriangleConstraint {
            std::vector<size_t> left_side;
            std::vector<size_t> right_side;
        };

        auto edge_constraints = std::vector<TriangleConstraint>(edge_count);

        if (cpx_logging) std::cout << "Got " << edge_count << " edges and " << n << " triangles" << std::endl;

        std::vector<bool> constraint_added(edge_count, false);

        auto get_edge_index = [this](Edge &edge) {
            auto search_result = utils::vector::binary_find(edges.begin(), edges.end(), edge->primary_edge());

            assert(search_result != edges.end());

            return std::distance(edges.begin(), search_result);
        };

        auto get_remaining_point = [](MWT::EmptyTriangle<Traits> &triangle, Edge edge) {
            for (auto &point: *triangle.get_points()) {
                if (point != edge->source() && point != edge->target()) {
                    return point;
                }
            }

            assert(false);
            return Point(0, 0);
        };

        auto add_triangle_to_edge_constraint =
                [this, &constraint_added, &get_edge_index, &get_remaining_point, &edge_constraints]
                        (size_t triangle_index, Edge &edge) {
                    auto edge_index = get_edge_index(edge);

                    //std::cout << triangle_index << " looked at " << edge_index << std::endl;

                    auto point = get_remaining_point(this->triangles[triangle_index], edge);

                    assert(edge->source() != point && edge->target() != point);

                    if (edge->primary_edge()->has_on_left_side(point)) {
                        edge_constraints[edge_index].left_side.emplace_back(triangle_index);
                    } else {
                        edge_constraints[edge_index].right_side.emplace_back(triangle_index);
                    }

                    constraint_added[edge_index] = true;
                };

        for (auto triangle : utils::pythonic::enumerate(triangles)) {
            add_triangle_to_edge_constraint(triangle.index, triangle.item.e1);
            add_triangle_to_edge_constraint(triangle.index, triangle.item.e2);
            add_triangle_to_edge_constraint(triangle.index, triangle.item.e3);

            assert((triangle.item.e1->source() != triangle.item.e2->target() &&
                    triangle.item.e1->target() != triangle.item.e2->target()) ||
                   (triangle.item.e1->source() != triangle.item.e2->source() &&
                    triangle.item.e1->target() != triangle.item.e2->source()));

            points.insert(triangle.item.get_points()->begin(), triangle.item.get_points()->end());

            assert(triangle.item.e1 != triangle.item.e2 &&
                   triangle.item.e1 != triangle.item.e3 &&
                   triangle.item.e2 != triangle.item.e3);

            objective_expr += triangle.item.weight() * vars[triangle.index];
        }


        for (auto edge: utils::pythonic::enumerate(edges)) {
            if (constraint_added[edge.index]) {
                IloNumExpr left_side_expression(env);

                auto *edge_constraint = &edge_constraints[edge.index];

                for (auto &i : edge_constraint->left_side) {
                    left_side_expression += this->vars[i];
                }

                if (edge_constraint->right_side.size() > 0
                    && edge_constraint->left_side.size() > 0) // inner edge or antenna
                {
                    if (edge.item->status == CGAL::Lmt_status::Possible) // inner edge
                    {
                        for (auto &i : edge_constraint->right_side) {
                            left_side_expression -= this->vars[i];
                        }
                        model.add(left_side_expression == 0);
                    } else // antenna
                    {
                        assert(edge.item->status == CGAL::Lmt_status::Certain &&
                               edge_constraint->right_side.size() > 0 &&
                               edge_constraint->left_side.size() > 0);

                        IloNumExpr right_side_expression(env);
                        for (auto &i : edge_constraint->right_side) {
                            right_side_expression += this->vars[i];
                        }
                        model.add(right_side_expression == 1);
                        model.add(left_side_expression == 1);
                    }
                } else if (edge_constraint->right_side.size() + edge_constraint->left_side.size() > 0) // boundary edge
                {
                    assert((edge_constraint->left_side.size() == 0 &&
                            edge_constraint->right_side.size() > 0) ||
                           (edge_constraint->left_side.size() > 0 &&
                            edge_constraint->right_side.size() == 0));

                    for (auto &i : edge_constraint->right_side) {
                        left_side_expression += this->vars[i];
                    }
                    model.add(left_side_expression == 1);
                } else {
                    std::cout << edge.item->is_certain()
                              << " found no triangle for edge even if constraints were added " << edge.index << ": "
                              << edge.item->source() << ", " << edge.item->target() << std::endl;
                }
            } else {
                std::cout << edge.item->is_certain() << " found no triangle for edge " << edge.index << ": "
                          << edge.item->source() << ", " << edge.item->target() << std::endl;
            }
        }


        model.add(IloMinimize(env, objective_expr));

        //cplex.exportModel("test.lp");
        objective_expr.end();

        construction_time = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start).count();
    }

    ~minimum_weight_triangulation_ip_solver() {
        /* cleans up all memory used by CPLEX */
        env.end();
    }

    bool is_lp_relaxation_integral()
    {
        IloModel lp_relaxation(env);
        lp_relaxation.add(model);
        lp_relaxation.add(IloConversion(env, vars, ILOFLOAT));

        IloCplex cplex_lp_relaxation(env);
        cplex_lp_relaxation.extract(lp_relaxation);
        cplex_lp_relaxation.solve();

        double epsilon =  0.001;
        bool integral = true;
        for (unsigned long v = 0; v < triangles.size(); ++v) {

            double current_val = cplex_lp_relaxation.getValue(vars[v]);
            //if(current_val > epsilon)
            //{
            //    std::cout << v << " " << current_val << std::endl;
            //
            //}

            if(current_val > epsilon && current_val < 1.0-epsilon)
            {
                integral = false;
            }
        }

        return integral;
    }

    result_type solve() {
        auto solution = triangle_container(0);
        auto objective_value = -1.0;
        auto solved = false;

        if (pre_solve_callback) pre_solve_callback(&env, &model, &cplex, &vars, triangle_generation_time, construction_time);

        if (cplex.solve()) {
            int triangle_count = 0;
            for (unsigned long v = 0; v < triangles.size(); ++v) {
                if (cplex.getValue(vars[v]) > 0.5) {
                    solution.push_back(triangles[v]);
                    triangle_count++;
                }
            }

            if (cpx_logging) std::cout << "#triangles in IP solution: " << triangle_count << std::endl;

            solved = true;
            objective_value = cplex.getObjValue();

            if (cpx_logging) std::cout << cplex.getObjValue() << " objective value" << std::endl;
        }

        if (post_solve_callback) post_solve_callback(&env, &model, &cplex, &vars, lazy_constrained_callback_time);

        return result_type{solution, objective_value, solved};
    }

    void
    register_pre_ip_solve_callback(std::function<void(IloEnv *, IloModel *, IloCplex *, IloBoolVarArray *, unsigned int, unsigned int)> callback) {
        pre_solve_callback = callback;
    }

    void
    register_post_ip_solve_callback(std::function<void(IloEnv *, IloModel *, IloCplex *, IloBoolVarArray *, unsigned int)> callback) {
        post_solve_callback = callback;
    }

private:
    using Point = typename Traits::Point_2;

    using size_t = std::size_t;
    using point_container = std::set<Point>;

    typedef cpxhelper::callback_container<minimum_weight_triangulation_ip_solver> Callbacks;

    triangle_container triangles;
    edge_container edges;
    point_container points;
    size_t n;
    size_t edge_count;

    IloEnv env;
    IloModel model;
    IloCplex cplex;
    IloBoolVarArray vars;

    std::function<void(IloEnv *, IloModel *, IloCplex *, IloBoolVarArray *, unsigned int, unsigned int)> pre_solve_callback;
    std::function<void(IloEnv *, IloModel *, IloCplex *, IloBoolVarArray *, unsigned int)> post_solve_callback;

    bool cpx_logging;

    Callbacks callbacks;
    std::mutex mutex;

    unsigned int triangle_generation_time;
    unsigned int construction_time;
    unsigned int lazy_constrained_callback_time;

    void lazy_constraint_callback(cpxhelper::lazy_constraint_callback *cb) {

        // std::rcout << "Entering Lazy " << points.size() << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        int constraints_added = 0;

        IloNumArray values;

        {
            std::lock_guard<std::mutex> lock(mutex);
            values = IloNumArray(cb->getEnv());
            cb->getValues(values, vars);
        }

        std::vector<size_t> triangles_in_solution;

        for (unsigned long i = 0; i < triangles.size(); ++i) {
            if (values[i] > 0.5) {
                triangles_in_solution.emplace_back(i);
            }
        }

        std::sort(triangles_in_solution.begin(), triangles_in_solution.end(),
                  [this](const size_t &i, const size_t &j) -> bool {
                      return triangles[i].leftmost_point() < triangles[j].leftmost_point();
                  }
        );

        std::vector<size_t> y_structure;

        for (auto &point : points) {
            for (auto it = y_structure.begin(); it != y_structure.end();)
                // Delete triangles which rightmost point is `point`
            {
                if (triangles[*it].rightmost_point() == point) {
                    it = y_structure.erase(it);
                } else {
                    it++;
                }
            }

            for (auto &i : y_structure) // Check if any triangle in the y structure contains `point`
            {
                if (triangles[i].contains_point(point)) {
                    std::lock_guard<std::mutex> lock(mutex);
                    cb->add(vars[i] == 0).end();
                    constraints_added++;
                }
            }

            for (auto it = triangles_in_solution.begin();
                 it != triangles_in_solution.end();) // Add new triangles which start at `point`
            {
                if (triangles[*it].leftmost_point() == point) {
                    y_structure.emplace_back(*it);
                    it = triangles_in_solution.erase(it);
                } else if (triangles[*it].leftmost_point().x() > point.x()) {
                    it = triangles_in_solution.end();
                } else {
                    it++;
                }
            }

        }

        lazy_constrained_callback_time += std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start).count();

        if(cpx_logging)
            std::cout << "Found " << constraints_added << " non empty triangles in solution" << std::endl;

    }
};

#endif //MWT_BASIC_MWT_SOLVER_HPP
