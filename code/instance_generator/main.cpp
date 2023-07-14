#include <iostream>
#include <vector>
#include <random>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio/ip/host_name.hpp>

#include "definitions/global_definitions.h"
#include "instance_generator/instance_random.h"
#include "utils/fileutils.hpp"
#include "utils/json_utils.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

int main(int argc, const char* argv[]) {

    boost::filesystem::path in;
    std::string type;
    std::size_t size = 0;
    uint count;
    std::vector<uint> interval;
    uint start;
    uint end;
    uint step = 1;
    std::string distribution;
    double mean;
    double stddev;
    int a;
    int b;
    bool json_output;
    boost::filesystem::path out;

    boost::program_options::options_description option_description("Allowed options");
    option_description.add_options()
            ("help,h", "Produce help message")
            ("file,f", boost::program_options::value<boost::filesystem::path>(&in), "Instance input file for converting to json format.")
            ("size,n", boost::program_options::value<std::size_t>(&size)->default_value(100),  "Size of random point set. Or to extract first n points from the input file.")
            ("count,c", boost::program_options::value<uint>(&count)->default_value(1), "Quantity of instances per size.")
            ("type", boost::program_options::value<std::string>(&type)->default_value(""), "Type of the input instance.")
            ("distribution", boost::program_options::value<std::string>(&distribution)->default_value("normal"), "Distribution type:\n"
                                                                                        "normal\n"
                                                                                        "uniform_int\n"
                                                                                        "uniform_real")
            ("mean", boost::program_options::value<double>(&mean)->default_value(0), "Mean for normal distribution")
            ("stddev", boost::program_options::value<double>(&stddev)->default_value(1), "Standard deviation for normal distribution")
            (",a", boost::program_options::value<int>(&a)->default_value(0), "'a' value for interval [a, b], resp. [a, b)")
            (",b", boost::program_options::value<int>(&b)->default_value(500), "'b' value for interval [a, b], resp. [a, b)")
            ("batch", boost::program_options::value<std::vector<uint>>(&interval)->multitoken(), "Define an interval for batch instance creation. (START END STEP)")
            ("json", boost::program_options::bool_switch(&json_output)->default_value(false), "Output as JSON file with additional metadata.")
            ("out,o", boost::program_options::value<boost::filesystem::path>(&out)->default_value("out_instances"), "Output directory.")
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

    if (options.count("batch")) {
        if (interval.size() != 3) {
            std::cout << "Three arguments needed! (START END STEP)" << std::endl;
            exit(1);
        }
        start = interval[0];
        end = interval[1];
        step = interval[2];
        std::cout << "Batch: " << start << " " << end << " " << step << std::endl;
    } else {
        start = size;
        end = size;
    }

    if (!boost::filesystem::is_directory(out)) {
        boost::filesystem::create_directory(out);
    }

    const int MAXLEN = 80;
    char s[MAXLEN];
    time_t t = time(0);
    strftime(s, MAXLEN, "%Y%m%d", localtime(&t));

    std::string prefix;
    if (options.count("file")) {
        prefix = boost::filesystem::basename(in) + "_";
    } else {
        prefix = boost::lexical_cast<std::string>(s) + "_" + distribution + "_";
    }

    for (uint n = start; n <= end; n+=step) {
        for (int i = 0; i < count; ++i) {

            uint seed = std::random_device{}();
            std::vector<Point_2> points;
            if (options.count("file")) {
                points = read_file<std::vector<Point_2>>(in);
            } else {
                if (distribution == "normal") {
                    points = instance_generator::normal_random(seed, n, mean, stddev);
                } else if (distribution == "uniform_int") {
                    points = instance_generator::uniform_int_random(seed, n, a, b);
                } else if (distribution == "uniform_real") {
                    points = instance_generator::uniform_real_random(seed, n, a, b);
                }
                std::sort(points.begin(), points.end());
                auto has_duplicates = std::adjacent_find(points.begin(), points.end()) != points.end();
                if(has_duplicates) {
                    std::cout << "Duplicate points found! " << n << " - " << i << std::endl;
                    i--;
                    continue;
                }
            }
            if (n < points.size()) {
                points.resize(n);
            }

            boost::filesystem::path fn{prefix
                    + boost::lexical_cast<std::string>(points.size())
                    + "_"
                    + boost::lexical_cast<std::string>(i)
                    };


            if (json_output) {
                json instance;
                instance["id"] = fn.string();
                if (type.size()) {
                    instance["type"] = type;
                } else {
                    instance["type"] = distribution;
                }
                instance["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                instance["host"] = boost::asio::ip::host_name();
                instance["size"] = points.size();
                instance["points"] = points_to_json(points);
                if (!options.count("file")) {
                    instance["seed"] = seed;
                    if (distribution == "normal") {
                        instance["normal_mean"] = mean;
                        instance["normal_stddev"] = stddev;
                    } else if (distribution == "uniform_int" || distribution == "uniform_real") {
                        instance["uniform_start"] = a;
                        instance["uniform_end"] = b;
                    }
                }
                fn += ".json";
                boost::filesystem::ofstream ostream{boost::filesystem::complete(fn, out)};
                ostream << instance.dump(4);
                ostream.close();
            } else {
                fn += ".instance";
                write_file(boost::filesystem::complete(fn, out), points);
            }
            std::cout << boost::filesystem::complete(fn, out) << std::endl;
        }
    }

}