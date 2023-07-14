#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "solver/Instance.h"
#include "solver/Solution.h"
#include "angle_tsp/Angle_tsp.h"
#include "mpp/Min_perimeter_polygon.h"
#include "alpha_mst/Alpha_mst.h"
#include "degree_mst/Degree_mst.h"

using json = nlohmann::json;

int main(int argc, const char* argv[]) {

    boost::filesystem::path in;
    std::string solver;
    bool json_output;
    bool plot;
    bool warm_start;
    bool scale;
    int time_limit;
    int degree;
    std::string instance;
    boost::filesystem::path ws_file;
    boost::filesystem::path out;
    std::vector<int> alpha_num_denom = std::vector<int>(2);

    boost::program_options::options_description option_description("Allowed options");
    option_description.add_options()
            ("help,h", "Produce help message")
            ("file,f", boost::program_options::value<boost::filesystem::path>(&in), "Instance input file.")
            ("solver", boost::program_options::value<std::string>(&solver), "Solver. Available:\n"
                                                                            "Angle_tsp\n"
                                                                            "Angle_tsp_lb\n"
                                                                            "Angle_distance_tsp\n"
                                                                            "Angle_distance_tsp_lb\n"
                                                                            "Mpp\n"
                                                                            "Alpha_mst\n"
                                                                            "Degree_mst")
            ("json", boost::program_options::bool_switch(&json_output)->default_value(false), "JSON output control.")
            ("plot", boost::program_options::bool_switch(&plot)->default_value(false), "GNUplot output control.")
            ("out,o", boost::program_options::value<boost::filesystem::path>(&out)->default_value("out_solver"),
             "Output directory.")
            ("ws,w", boost::program_options::bool_switch(&warm_start)->default_value(false), "Toggle warm start.\n"
                                                                                             "MPP: warm start by solving on delaunayT")
            ("ws_file", boost::program_options::value<boost::filesystem::path>(&ws_file), "WarmStart from json solution file.")
            ("alpha", boost::program_options::value<std::vector<int>>(&alpha_num_denom)->multitoken(),
             "Numerator x and denominator y to define alpha: (x*M_PI)/y")
            ("timelimit,t", boost::program_options::value<int>(&time_limit)->default_value(0),
             "Solver time limit. 0 for no time limit.")
            ("degree, d", boost::program_options::value<int>(&degree)->default_value(0),
             "Degree for degree constrained solver.")
            ("scale", boost::program_options::bool_switch(&scale)->default_value(false), "Scale instance to square of width and height 500");

    boost::program_options::variables_map options;

    boost::program_options::positional_options_description p;

    try {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
                                              options(option_description).
                                              positional(p).
                                              run(),
                                      options);
        boost::program_options::notify(options);
    } catch (const boost::program_options::error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (options.count("help") || options.count("h")) {
        std::cout << option_description;
        return 0;
    }

    if (!options.count("file")) {
        std::cerr << "file missing" << std::endl;
        std::cout << option_description;
        return 0;
    }

    if (!options.count("solver")) {
        std::cerr << "solver missing" << std::endl;
        std::cout << option_description;
        return 0;
    }

    Solver::Instance solver_instance;
    solver_instance = Solver::Instance{in, scale};

    std::vector<std::pair<std::size_t, std::size_t>> ws_edges;
    if (options.count("ws_file")) {
        json json_instance_file;
        boost::filesystem::ifstream ifs{ws_file};
        ifs >> json_instance_file;
        ifs.close();
        for (auto e : json_instance_file["solution_edges"]) {
            ws_edges.emplace_back(e[0], e[1]);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::shared_ptr<Solver::Solution> solution;

    if (solver == "Angle_tsp") {
        angle_tsp::Angle_tsp angle_tsp{solver_instance, angle_tsp::ANGLE, true, false, time_limit};
        solution = angle_tsp.getSolution();
    } else if (solver == "Angle_tsp_lb") {
        angle_tsp::Angle_tsp angle_tsp{solver_instance, angle_tsp::ANGLE_LB, false, false, time_limit};
        solution = angle_tsp.getSolution();
    } else if (solver == "Angle_distance_tsp") {
        angle_tsp::Angle_tsp angle_dist_tsp{solver_instance, angle_tsp::ANGLE_DISTANCE, true, false, time_limit};
        solution = angle_dist_tsp.getSolution();
    } else if (solver == "Angle_distance_tsp_lb") {
        angle_tsp::Angle_tsp angle_dist_tsp{solver_instance, angle_tsp::ANGLE_DISTANCE_LB, false, false, time_limit};
        solution = angle_dist_tsp.getSolution();
    } else if (solver == "Mpp") {
        mpp::Min_perimeter_polygon minPerimeterPolygon{solver_instance, mpp::MPP, ws_edges, true, warm_start, time_limit};
        solution = minPerimeterPolygon.getSolution();
    } else if (solver == "Alpha_mst") {
        double alpha = (alpha_num_denom[0] * M_PI) / alpha_num_denom[1];
        amst::Alpha_mst alphaMst{solver_instance, amst::ALPHA_MST, alpha, time_limit};
        solution = alphaMst.getSolution();
    } else if (solver == "Degree_mst") {
        dmst::Degree_mst degreeMst{solver_instance, dmst::DEGREE_MST, degree, warm_start, time_limit};
        solution = degreeMst.getSolution();
    } else {
        std::cout << "No available solver chosen!" << std::endl;
        return 0;
    }

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    std::string fn = solver_instance.instance_name + "_" + solver;

    if (json_output) {
        solution->to_json(boost::filesystem::complete(fn + ".json" , out).string());
    }

    if (plot) {
        solution->plot(boost::filesystem::complete(fn, out).string());
    }

    return 0;
}