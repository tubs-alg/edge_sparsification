add_library(libsparsification
        src/edge_sparsification/EdgeSparsification.cpp
        src/edge_sparsification/sparsification.cpp
        src/greedy_triangulation/GreedyTriangulation.cpp
        src/wspd/SplittreeNode.cpp
        src/wspd/Splittree.cpp
        src/wspd/WSPD.cpp
        src/greedy_spanner/GreedySpanner.cpp
        src/onion_hull/OnionHull.cpp
        src/gabriel/GabrielGraph.cpp
        src/rng/RelativeNeighborhoodGraph.cpp
        )

set_target_properties(libsparsification PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(libsparsification
        PUBLIC
        ${CMAKE_BASE_DIR}/definitions/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_BASE_DIR}/mwt/mwt_library/include
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${GUROBI_INCLUDE_DIRS}
        )

target_link_libraries(libsparsification
        libinstance_generator
        algutil::algcplex
        mwt_library
        )

add_executable(edge_sparsification main.cpp)

target_link_libraries(edge_sparsification
        libsparsification
        ${BOOST_LIBRARIES}
        ${CGAL_LIBRARIES}
        ${GUROBI_LIBRARIES}
        )

target_include_directories(edge_sparsification
        PUBLIC
        ${CMAKE_BASE_DIR}/definitions/include
        ${CMAKE_BASE_DIR}/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

set_target_properties(edge_sparsification PROPERTIES LINKER_LANGUAGE CXX)

add_executable(edge_sparsification_merge merge_main.cpp)

target_link_libraries(edge_sparsification_merge
        libsparsification
        ${BOOST_LIBRARIES}
        ${CGAL_LIBRARIES}
        ${GUROBI_LIBRARIES}
        )

target_include_directories(edge_sparsification_merge
        PUBLIC
        ${CMAKE_BASE_DIR}/definitions/include
        ${CMAKE_BASE_DIR}/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_BASE_DIR}/mwt
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

set_target_properties(edge_sparsification_merge PROPERTIES LINKER_LANGUAGE CXX)