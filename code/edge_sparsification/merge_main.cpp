#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "edge_sparsification/EdgeSparsification.h"

#include "definitions/global_definitions.h"

int main(int argc, const char* argv[]) {

    boost::filesystem::path in1;
    boost::filesystem::path in2;
    bool dilation;
    bool stretch_analysis;
    bool json_output;
    bool plot;
    boost::filesystem::path out;

    boost::program_options::options_description option_description("Allowed options");
    option_description.add_options()
            ("help,h", "Produce help message")
            ("file1", boost::program_options::value<boost::filesystem::path>(&in1)->required(), "Instance input file.")
            ("file2", boost::program_options::value<boost::filesystem::path>(&in2)->required(), "Instance input file.")
            ("dilation,d", boost::program_options::bool_switch(&dilation)->default_value(false), "Computing the spanning ratio.")
            ("stretch_analysis", boost::program_options::bool_switch(&stretch_analysis)->default_value(false), "Spanning ratio for all point pairs.")
            ("json", boost::program_options::bool_switch(&json_output)->default_value(false), "JSON output control.")
            ("plot", boost::program_options::bool_switch(&plot)->default_value(false), "GNUplot output control.")
            ("out,o", boost::program_options::value<boost::filesystem::path>(&out)->default_value("out_merge"), "Output directory.")
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

    Sparsification::EdgeSparsification sparsification_instance = Sparsification::from_json(in1);
    Sparsification::EdgeSparsification sparsification_instance_2 = Sparsification::from_json(in2);

    if (dilation) {
        sparsification_instance.compute_stretch(false);
        sparsification_instance_2.compute_stretch(false);
    }
    std::cout << "First input: " << std::endl;
    sparsification_instance.print();
    std::cout << "Second input: " << std::endl;
    sparsification_instance_2.print();

    sparsification_instance.mergeEdges(sparsification_instance_2);
    if (dilation || stretch_analysis) {
        sparsification_instance.compute_stretch(stretch_analysis);
    }
    sparsification_instance.print();
    sparsification_instance.to_json(out);
    sparsification_instance.plot(out);

    return 0;
}