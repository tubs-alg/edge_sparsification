add_executable(genetic genetic.cpp)

target_include_directories(genetic
        PUBLIC
        ${CMAKE_BASE_DIR}/edge_sparsification/include
        ${CMAKE_BASE_DIR}/solver/include
        ${CMAKE_BASE_DIR}/definitions/include
        ${CMAKE_BASE_DIR}/utils/include
        )

target_link_libraries(genetic
        libsparsification
        libsolver
        ${BOOST_LIBRARIES}
        ${GUROBI_LIBRARIES}
        ${CGAL_LIBRARIES}
        )