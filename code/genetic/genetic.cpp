#ifndef SPARSIFICATION_GENETIC_CPP
#define SPARSIFICATION_GENETIC_CPP

#include <vector>
#include <random>
#include <algorithm>
#include <csignal>
#include <boost/program_options.hpp>

#include "fitness.h"
#include "definitions/global_definitions.h"
#include "utils/fileutils.hpp"

const std::size_t POPULATION_SIZE = 1000;
const std::size_t MAX_ITERATIONS = 5000;
const std::size_t MAX_STAGNATION = 100;
const double SURVIVAL_RATE = 0.2;
const double COORDINATE_MAX = 100.0;
const double MUTATION_IMPACT_STDDEV = 0.02;
const double MUTATION_NUM_STDDEV = 2.0;

double current_best = 0;
std::vector<Point_2> current_best_instance;

void signal_handler(int signum) {
    std::cout << std::endl << "best instance: " << current_best << std::endl;
    std::cout.precision(17);
    for (auto & p : current_best_instance) {
        std::cout << p << std::endl;
    }
    exit(0);
}

bool vector_has_duplicates(std::vector<Point_2> & _points) {
    std::sort(_points.begin(), _points.end());
    return std::adjacent_find(_points.begin(), _points.end()) != _points.end();
}

std::vector<Point_2> mutate(std::vector<Point_2> & parent, std::size_t num, double impact) {

    std::size_t num_points = parent.size();
    num = std::min(num_points, num);

    double mean_dis = 0;
    for (int i = 0; i < num_points; ++i) {
        for (int j = i + 1; j < num_points; ++j) {
            mean_dis += std::sqrt(CGAL::squared_distance(parent[i], parent[j]));
        }
    }
    mean_dis = mean_dis / (num_points * (num_points - 1) / 2);

    std::vector<Point_2> child{parent};

    auto rng = std::default_random_engine {};
    std::shuffle(child.begin(), child.end(), rng);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 2 * M_PI);

    for (int i = 0; i < num; ++i) {
        double angle = dis(gen);
        double x = mean_dis * impact * std::cos(angle);
        double y = mean_dis * impact * std::sin(angle);

        child[i] = Point_2(child[i].x() + x, child[i].y() + y);
    }

    return child;
}

void build_random_population(std::vector<std::vector<Point_2>> & population, std::size_t pop_size, std::size_t instance_size) {
    population.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, COORDINATE_MAX);

    for (int i = 0; i < pop_size; ++i)
    {
        std::vector<Point_2> instance;
        for (int j = 0; j < instance_size; ++j) {
            instance.emplace_back(dis(gen), dis(gen));
        }
        if (!vector_has_duplicates(instance)) {
            population.push_back(instance);
        } else {
            --i;
        }
    }
}

void read_population(std::vector<std::vector<Point_2>> & population, boost::filesystem::path in, std::size_t pop_size) {
    for (int i = 0; i < pop_size; ++i) {
        std::vector<Point_2> instance = read_file<std::vector<Point_2>>(in);
        population.push_back(instance);
    }
}

int main(int argc, const char* argv[])
{
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    std::vector<std::vector<Point_2>> current_population;
    std::vector<std::vector<Point_2>> next_population;

    int variant;
    boost::filesystem::path in;
    std::size_t instance_size;

    boost::program_options::options_description option_description("Allowed options");
    option_description.add_options()
            ("help,h", "Produce help message")
            ("file,f", boost::program_options::value<boost::filesystem::path>(&in), "Instance input file.")
	    ("size,s", boost::program_options::value<std::size_t>(&instance_size)->default_value(10), "Instance size.")
            ("variant,v", boost::program_options::value<int>(&variant)->default_value(0), "Available variants:\n"
                                                                                               "0 : Spanning Ratio MWT\n"
                                                                                               "1 : Degree Mst Complete vs. Gabriel Graph\n")
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

    int size;
    if (options.count("file")) {
        read_population(current_population, in, POPULATION_SIZE);
        size = current_population[0].size();
    } else {
        //
        //build initial population
        //
        build_random_population(current_population, POPULATION_SIZE, instance_size);
        size = instance_size;
    }

    int iteration = 0;
    double last_val = 0;
    int stagnation_counter = 0;
    do {
        //
        //determine fitness and selection probability of all instances
        //
        double min_fitness = std::numeric_limits<double>::max();
        double max_fitness = std::numeric_limits<double>::lowest();
        double fitness_sum = 0;
        std::vector<double> fitness_values;
        for (auto &instance : current_population) {
            double fitness;
            switch (variant) {
                case 0:
                    fitness = fitness_stretch(instance);
                    break;
                case 1:
                    fitness = fitness_degree_mst(instance);
                    break;
                case 2:
                    fitness = fitness_delaunay_mwt_gap(instance);
            }
            fitness_values.push_back(fitness);
            min_fitness = std::min(min_fitness, fitness);
            max_fitness = std::max(max_fitness, fitness);

            if (fitness > current_best) {
                current_best = fitness;
                current_best_instance = instance;

                std::cout << "iteration " << iteration << ", max fitness value: " << max_fitness << std::endl;
            }
        }

        if (max_fitness == last_val) {
            ++stagnation_counter;
        } else {
            stagnation_counter = 0;
            last_val = max_fitness;
        }

        for (auto &f : fitness_values) {
            f = f - min_fitness;
            fitness_sum += f;
        }

        std::vector<double> probabilities;
        std::vector<double> rank_selection_probabilities;
        double probabilities_sum = 0;
        for (int i = 0; i < fitness_values.size(); ++i) {
            double probability = fitness_values[i] / fitness_sum;
            probabilities_sum += probability;
            probabilities.push_back(probabilities_sum);
            rank_selection_probabilities.emplace_back(1 / (i+1));
        }

        //
        // selection
        //
        std::random_device rd_2;
        std::mt19937 gen_2(rd_2());
        std::uniform_real_distribution<double> dis_2(0.0, 1.0);
        std::normal_distribution<double> mutation_impact_dist(0.0, MUTATION_IMPACT_STDDEV);
        std::normal_distribution<double> mutation_number(0.0, MUTATION_NUM_STDDEV);
        std::discrete_distribution<std::size_t> rank_selection(rank_selection_probabilities.begin(),
                                                               rank_selection_probabilities.end());

        next_population.clear();

        int current_survivale_rate = std::ceil(SURVIVAL_RATE * POPULATION_SIZE);
        if (stagnation_counter > MAX_STAGNATION) {
            stagnation_counter = 0;
            last_val = 0;
            if (options.count("file")) {
                read_population(next_population, in, POPULATION_SIZE);
            } else {
                build_random_population(next_population, POPULATION_SIZE, instance_size);
            }
            std::cout << "Iteration " << iteration << ", stagnation limit exceeded. Building new population..." << std::endl;
        } else {
            // put best x percent without mutation to next population
            std::vector<int> sorted_indices(POPULATION_SIZE, 0);
            std::iota(sorted_indices.begin(), sorted_indices.end(), 0);
            std::sort(sorted_indices.begin(), sorted_indices.end(),
                      [&](const int &a, const int &b) {
                          return (fitness_values[a] > fitness_values[b]);
                      }
            );

            for (int i = 0; i < current_survivale_rate; ++i) {
                next_population.push_back(current_population[sorted_indices[i]]);
            }

            for (int i = current_survivale_rate; i < POPULATION_SIZE; ++i) {

                int parent = 0;
                std::vector<Point_2> child;
                do {
                    // roulette wheel selection
                    double parent_selection = dis_2(gen_2);
                    while (parent_selection > probabilities[parent]) {
                        ++parent;
                    }
//                    parent = sorted_indices[rank_selection(gen_2)]; // rank selection

                    child = mutate(current_population[parent],
                                   std::max(1, (int) std::abs(std::round(mutation_number(gen_2)))),
                                   std::abs(mutation_impact_dist(gen_2)));
                    next_population.push_back(child);

                } while (vector_has_duplicates(child));
            }
        }

        auto tmp = current_population;
        current_population = next_population;
        next_population = tmp;
        ++iteration;
//    } while (iteration < MAX_ITERATIONS);
    } while (true);

    signal_handler(0);

}


#endif //SPARSIFICATION_GENETIC_CPP
