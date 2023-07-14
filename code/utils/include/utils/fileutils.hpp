#ifndef SPARSIFICATION_FILEUTILS_HPP
#define SPARSIFICATION_FILEUTILS_HPP

#include <boost/filesystem.hpp>
#include <iostream>

template<class Container>
Container read_file(boost::filesystem::path file) {
    if(!boost::filesystem::is_regular_file(file)) {
        throw std::invalid_argument("Not a file: " + file.string());
    }

    boost::filesystem::ifstream stream{file};

    std::istream_iterator<typename Container::value_type> start{stream};
    std::istream_iterator<typename Container::value_type> end{};

    Container container{start, end};

    return container;
}

template<class Container>
void write_file(boost::filesystem::path file, Container points) {
    boost::filesystem::ofstream stream{file};

    stream << std::setprecision(16);
    for (auto p : points) {
        stream << p.x() << " " << p.y() << std::endl;
    }

    stream.close();

}

#endif //SPARSIFICATION_FILEUTILS_HPP
