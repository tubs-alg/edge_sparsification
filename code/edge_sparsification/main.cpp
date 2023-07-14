#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <edge_sparsification/EdgeSparsification.h>
#include <edge_sparsification/sparsification.h>

int main(int argc, const char* argv[]) {

    std::size_t size = 0;
    std::size_t k = 0;
    std::size_t seed = 0;
    boost::filesystem::path in;
    std::string algorithm;
    int cones_selected;
    int variant;
    bool dilation;
    bool stretch_analysis;
    bool json_output;
    bool plot;
    double t;
    double sep_factor;
    boost::filesystem::path out;

    boost::program_options::options_description option_description("Allowed options");
    option_description.add_options()
            ("help,h", "Produce help message")
            ("file,f", boost::program_options::value<boost::filesystem::path>(&in), "Instance input file.")
            ("size,n", boost::program_options::value<std::size_t>(&size)->default_value(100),  "Size of random point set.")
            ("k,k", boost::program_options::value<std::size_t>(&k), "Parameter k, e.g. for theta and yao graph construction.")
            ("seed,s", boost::program_options::value<std::size_t>(&seed)->default_value(std::random_device{}()),  "Random Seed.")
            ("dilation,d", boost::program_options::bool_switch(&dilation)->default_value(false), "Computing the max spanning ratio.")
            ("stretch_analysis", boost::program_options::bool_switch(&stretch_analysis)->default_value(false), "Spanning ratio for all point pairs.")
            ("algorithm,a", boost::program_options::value<std::string>(&algorithm)->default_value("complete"), "Algorithm used for edge sparsification. Available:\n"
                                                                                                                    "delaunayT : Delaunay triangulation\n"
                                                                                                                    "greedyT : Greedy triangulation\n"
                                                                                                                    "greedyTMax : Max greedy triangulation\n"
                                                                                                                    "mwT : Minimum weight triangulation\n"
                                                                                                                    "yao : Yao-k graph\n"
                                                                                                                    "theta : Theta-k graph\n"
                                                                                                                    "wspd : Well-Separated-Pair-Decomposition\n"
                                                                                                                    "greedy : greedy t-spanner\n"
                                                                                                                    "onion : convex layer based sparsification\n"
                                                                                                                    "complete : complete graph\n")
            ("cones,c", boost::program_options::value<int>(&cones_selected)->default_value(2), "Choice of cones in cone spanners. Can be:\n"
                                                                                                                    "2 : ALL CONES\n"
                                                                                                                    "1 : ODD CONES\n"
                                                                                                                    "0 : EVEN_CONES\n")
            ("variant,v", boost::program_options::value<int>(&variant)->default_value(0), "INT parameter if there are different variants available, e.g. for WSPD")
            ("json", boost::program_options::bool_switch(&json_output)->default_value(false), "JSON output control.")
            ("plot", boost::program_options::bool_switch(&plot)->default_value(false), "GNUplot output control.")
            (",t", boost::program_options::value<double>(&t)->default_value(2), "Parameter t, e.g. for t-spanner construction. t > 1")
            ("sep", boost::program_options::value<double>(&sep_factor), "Separation factor for WSPD. Parameter t is ignored.")
            ("out,o", boost::program_options::value<boost::filesystem::path>(&out)->default_value("out_sparsification"), "Output directory.")
            ;

    boost::program_options::variables_map options;

    boost::program_options::positional_options_description p;

    try {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
            options(option_description).
            positional(p).
            run(),
            options);
        boost::program_options::notify(options);
    } catch(const boost::program_options::error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (options.count("help") || options.count("h")) {
        std::cout << option_description;
        return 0;
    }

    Sparsification::EdgeSparsification sparsification_instance;

    if (options.count("file")) {
        sparsification_instance = Sparsification::EdgeSparsification{in};
    } else {
        sparsification_instance = Sparsification::EdgeSparsification{seed, size, "normal"};
    }

    auto start = std::chrono::high_resolution_clock::now();

    if (algorithm == "complete") {

        Sparsification::complete_graph(sparsification_instance);

    } else if (algorithm == "delaunayT") {

        Sparsification::delaunay_triangulation(sparsification_instance);

    } else if (algorithm == "theta") {

        Sparsification::theta(sparsification_instance, cones_selected, k);

    } else if (algorithm == "yao") {

        Sparsification::yao(sparsification_instance, cones_selected, k);

    } else if (algorithm == "wspd") {

        if (!options.count("sep")) {
            Sparsification::wspd_spanner(sparsification_instance, t, variant);
        } else {
            Sparsification::wspd_spanner(sparsification_instance, t, variant, sep_factor);
        }

    } else if (algorithm == "greedyT") {

        Sparsification::greedy_triangulation(sparsification_instance);

    } else if (algorithm == "greedyTMax") {

        Sparsification::greedy_triangulation(sparsification_instance, false);

    } else if (algorithm == "mwT") {

        Sparsification::minimum_weight_triangulation(sparsification_instance);

    } else if (algorithm == "greedy") {

        Sparsification::greedy_spanner(sparsification_instance, t);

    } else if (algorithm == "onion") {

        Sparsification::onion(sparsification_instance, variant, k);

    } else if (algorithm == "gabriel") {

        Sparsification::gabriel_graph(sparsification_instance);

    } else if (algorithm == "rng") {

        Sparsification::relative_neighborhood_graph(sparsification_instance);

    }

    if (dilation || stretch_analysis) {
        sparsification_instance.compute_stretch(stretch_analysis);
    }
    sparsification_instance.print();

    if (json_output) {
        sparsification_instance.to_json(out, !stretch_analysis, !stretch_analysis);
    }
    if (plot) {
        sparsification_instance.plot(out);
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    return 0;
}