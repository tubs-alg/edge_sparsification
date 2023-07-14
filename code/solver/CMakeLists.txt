add_library(libsolver
        src/solver/Instance.cpp
        src/angle_tsp/Angle_tsp.cpp
        src/angle_tsp/Angle_tsp_solution.cpp
        src/alpha_mst/Alpha_mst.cpp
        src/alpha_mst/Alpha_mst_solution.cpp
        src/degree_mst/Degree_mst.cpp
        src/degree_mst/Degree_mst_solution.cpp
        src/mpp/Min_perimeter_polygon.cpp
        src/mpp/Min_perimeter_polygon_solution.cpp
        )

#set_target_properties(libsolver PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(libsolver
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

target_link_libraries(libsolver
        libsparsification
        ${GUROBI_LIBRARIES}
        ${CGAL_LIBRARIES}
        )

add_executable(solver main.cpp)

target_link_libraries(solver
        libsolver
        ${BOOST_LIBRARIES}
        )