#include "edge_sparsification/sparsification.h"

void Sparsification::complete_graph(Sparsification::EdgeSparsification &sparsification_instance) {
    for (std::size_t i = 0; i < sparsification_instance.num_points(); ++i) {
        for (std::size_t j = i+1; j < sparsification_instance.num_points(); ++j) {
            sparsification_instance.insertEdge(i, j);
        }
    }
    sparsification_instance.setSparsificationType("complete");
}

void Sparsification::delaunay_triangulation(Sparsification::EdgeSparsification &sparsification_instance) {
    DelaunayTriangulation dt =  {sparsification_instance.points.begin(), sparsification_instance.points.end()};

    for (auto it = dt.finite_edges_begin(); it != dt.finite_edges_end(); ++it) {
        Segment_2 seg = dt.segment(it);
        sparsification_instance.insertEdge(seg.source(), seg.target());
    }
    sparsification_instance.setSparsificationType("delaunayT");
}

void Sparsification::greedy_triangulation(Sparsification::EdgeSparsification &sparsification_instance, bool min) {
    GreedyTriangulation gt = GreedyTriangulation{sparsification_instance, min};
    if (min) {
        sparsification_instance.setSparsificationType("greedyT");
    } else {
        sparsification_instance.setSparsificationType("greedyTMax");
    }
}

void Sparsification::minimum_weight_triangulation(Sparsification::EdgeSparsification &sparsification_instance) {
    MWT::MWTSolver::return_type_epick result = MWT::MWTSolver().solve(sparsification_instance.points);

    for (auto & mwt_edge : result.edges) {
        sparsification_instance.insertEdge(mwt_edge.first, mwt_edge.second);
    }
    sparsification_instance.setSparsificationType("mwT");
}

void Sparsification::theta(Sparsification::EdgeSparsification &sparsification_instance, int cones_selected, int k) {
    Graph g_theta;
    CGAL::Construct_theta_graph_2<Kernel, Graph> theta(k, Direction_2{1,0},
                                                       static_cast<CGAL::Cones_selected>(cones_selected));
    theta(sparsification_instance.points.begin(), sparsification_instance.points.end(), g_theta);
    auto edges = boost::edges(g_theta);
    for (auto e = edges.first; e != edges.second; ++e) {
        auto s_i = boost::source(*e,g_theta);
        auto t_i = boost::target(*e,g_theta);
        sparsification_instance.insertEdge(s_i, t_i);
    }
    std::string algorithm = "theta" + boost::lexical_cast<std::string>(k) + "-" + boost::lexical_cast<std::string>(cones_selected);
    sparsification_instance.setSparsificationType(algorithm);
}

void Sparsification::yao(Sparsification::EdgeSparsification &sparsification_instance, int cones_selected, int k) {
    Graph g_yao;
    CGAL::Construct_yao_graph_2<Kernel, Graph> yao(k, Direction_2{1,0},
                                                   static_cast<CGAL::Cones_selected>(cones_selected));
    yao(sparsification_instance.points.begin(), sparsification_instance.points.end(), g_yao);
    auto edges = boost::edges(g_yao);
    for (auto e = edges.first; e != edges.second; ++e) {
        auto s_i = boost::source(*e,g_yao);
        auto t_i = boost::target(*e,g_yao);
        sparsification_instance.insertEdge(s_i, t_i);
    }
    std::string algorithm = "yao" + boost::lexical_cast<std::string>(k) + "-" + boost::lexical_cast<std::string>(cones_selected);
    sparsification_instance.setSparsificationType(algorithm);
}

void Sparsification::greedy_spanner(Sparsification::EdgeSparsification &sparsification_instance, double t) {
    Sparsification::GreedySpanner greedySpanner{sparsification_instance, t};
    sparsification_instance.setSparsificationType("greedy" + boost::lexical_cast<std::string>(t));
}

void Sparsification::wspd_spanner(Sparsification::EdgeSparsification &sparsification_instance, double t, int variant,
                                  double sep) {
    std::string algorithm = "wspd";
    if (sep < 0) {
        sep = 4 * (t + 1) / (t - 1);
        algorithm += boost::lexical_cast<std::string>(t) + '-' + boost::lexical_cast<std::string>(variant);
    } else {
        algorithm += boost::lexical_cast<std::string>(0) + '-' + boost::lexical_cast<std::string>(sep) + '-' + boost::lexical_cast<std::string>(variant);
    }
    Sparsification::WSPD::WSPD wspd{sparsification_instance, sep};
    wspd.computeGraph(variant);
    sparsification_instance.json_s["s"] = sep;
    sparsification_instance.setSparsificationType(algorithm);
}

void Sparsification::onion(Sparsification::EdgeSparsification &sparsification_instance, int variant, int k) {
    Sparsification::OnionHull onion{sparsification_instance};
    onion.connectLayers(variant, k);
    std::string algorithm = "onion-" + boost::lexical_cast<std::string>(variant);
    if (k > 0) {
        algorithm += '-' + boost::lexical_cast<std::string>(k);
    }
    sparsification_instance.setSparsificationType(algorithm);
}

void Sparsification::gabriel_graph(Sparsification::EdgeSparsification &sparsification_instance) {
    Sparsification::GabrielGraph gg{sparsification_instance};
    sparsification_instance.setSparsificationType("gabrielGraph");
}

void Sparsification::relative_neighborhood_graph(Sparsification::EdgeSparsification &sparsification_instance) {
    Sparsification::RelativeNeighborhoodGraph rng{sparsification_instance};
    sparsification_instance.setSparsificationType("relativeNeighborhoodGraph");
}
