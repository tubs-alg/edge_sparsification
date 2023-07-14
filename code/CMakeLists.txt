cmake_minimum_required(VERSION 3.7)
project(sparsification)

set(CMAKE_CXX_STANDARD 17)
set(UTIL_CXX_STANDARD 17)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(CXX "enable C++ compilation" ON)
if(CXX)
    enable_language(CXX)
endif()

set(UTIL_BOOST_LIBRARIES filesystem program_options)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake" NO_POLICY_SCOPE)

set(_CPLEX_CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Gurobi Home: $ENV{GUROBI_HOME}")

set(CMAKE_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(GUROBI REQUIRED)
find_package(Boost REQUIRED program_options filesystem)
find_package(CGAL REQUIRED)
find_package(CPLEX REQUIRED)

set(GUROBI_LIBRARIES
        optimized ${GUROBI_CXX_LIBRARY}
        debug ${GUROBI_CXX_DEBUG_LIBRARY}
        ${GUROBI_LIBRARY}
        )

set(BOOST_LIBRARIES
        Boost::program_options
        Boost::filesystem
        )

set(CGAL_LIBRARIES
        CGAL::CGAL
        )

# misc
add_subdirectory(instance_generator)

# sparsification
add_subdirectory(mwt)
add_subdirectory(edge_sparsification)

# solver
add_subdirectory(solver)

add_subdirectory(genetic)


if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    include(FeatureSummary)
    feature_summary(WHAT ALL)
endif()
