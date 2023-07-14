include("${CMAKE_CURRENT_LIST_DIR}/MWTTargets.cmake")

if(NOT CGAL_COMPLETELY_ADDED)
    find_package(CGAL QUIET COMPONENTS Core)
    include(${CGAL_USE_FILE})
endif()

target_link_libraries(MWT::mwt_library INTERFACE algutil::algcplex CGAL gmp mpfr )