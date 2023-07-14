#ifndef MWT_IP_SOLVER_UTILS_H
#define MWT_IP_SOLVER_UTILS_H

#include <vector>

#include "mwt/utils/vector_utils.hpp"
#include "mwt/utils/progress_bar.h"
#include "mwt/CPLEX/empty_triangle.hpp"
#include <mutex>
#include <thread>


namespace utils::ip_solver
{

    template <typename Halfedge_handle>
    Halfedge_handle primary_triangle_edge(Halfedge_handle e)
    {
        std::vector<Halfedge_handle> possible_edges = {e, e->j, e->i->twin()};

        std::sort(possible_edges.begin(), possible_edges.end());

        for (auto edge: possible_edges) {
            if (edge->status == CGAL::Lmt_status::Possible) {
                return edge;
            }
        }

        assert(true); //Something went wrong before
        return nullptr;
    }

    template <typename Traits, typename Halfedge_handle>
    void get_empty_triangles(Halfedge_handle e, std::set<Halfedge_handle> &all_edges, std::vector<MWT::EmptyTriangle<Traits>> &triangles, std::mutex *mutex) {
        auto i_old = e->i; // not necessary
        auto j_old = e->j; // not necessary
        e->reset();

        CGAL_assertion(all_edges.find(e->primary_edge()) != all_edges.end());

        while (e->next_triangle()) {

            if(all_edges.find(e->i->primary_edge()) == all_edges.end() || all_edges.find(e->j->primary_edge()) == all_edges.end())
            {
                continue; // We have found a triangle which is not part of the face.
            }

            Halfedge_handle primary_edge = primary_triangle_edge<Halfedge_handle>(e);


            CGAL_assertion(primary_edge != nullptr &&
            (primary_edge == e || primary_edge == e->i->twin() || primary_edge == e->j));

            if (e == primary_edge) {
                std::lock_guard<std::mutex> lock(*mutex);
                triangles.emplace_back(MWT::EmptyTriangle<Traits>(e));
            }
        }
        e->i = i_old; // not necessary
        e->j = j_old; // not necessary
    }

    template <typename Traits, typename Halfedge_handle, typename Face>
    void get_empty_triangles_thread(std::vector<MWT::EmptyTriangle<Traits>> &triangles,
                                    Face &face,
                                    std::set<Halfedge_handle> &all_edges,
                                    utils::ProgressBar &pb,
                                    int thread,
                                    int thread_count,
                                    std::mutex *mutex,
                                    bool log = false) {

        for (unsigned int i = thread; i < face.inner_edges.size(); i+=thread_count) {
            assert(face.inner_edges[i]->status == CGAL::Lmt_status::Possible && face.inner_edges[i]->twin()->status == CGAL::Lmt_status::Possible);

            get_empty_triangles<Traits, Halfedge_handle>(face.inner_edges[i], all_edges, triangles, mutex);

            if (log)
            {
                std::lock_guard<std::mutex> lock(*mutex);
                pb++;
            }
        }

    }

    template <typename Traits, typename Halfedge_handle, typename Face>
    std::vector<MWT::EmptyTriangle<Traits>> get_empty_triangles(Face &face, std::set<Halfedge_handle> &all_edges, std::mutex *mutex, bool log = false) {
        std::vector<MWT::EmptyTriangle<Traits>> triangles;

        utils::ProgressBar pb(face.inner_edges.size());

        std::vector<std::thread> threads;
        int thread_count = 4;

        for(int i = 0; i<thread_count; i++)
        {
            threads.emplace_back(get_empty_triangles_thread<Traits, Halfedge_handle, Face>,
                                    std::ref(triangles),
                                    std::ref(face),
                                    std::ref(all_edges),
                                    std::ref(pb),
                                    i,
                                    thread_count,
                                    mutex,
                                    log);
        }

        for(auto &t : threads)
        {
            t.join();
        }

        return triangles;
    }
}

#endif //MWT_IP_SOLVER_UTILS_H
