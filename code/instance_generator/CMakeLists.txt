add_library(libinstance_generator
        src/instance_generator/instance_random.cpp
        )

set_target_properties(libinstance_generator PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(libinstance_generator
        PUBLIC
        ${CMAKE_BASE_DIR}/definitions/include
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

add_executable(instance_random main.cpp)

target_link_libraries(instance_random
        libinstance_generator
        ${BOOST_LIBRARIES}
        ${CGAL_LIBRARIES}
        )

target_include_directories(instance_random
        PUBLIC
        ${CMAKE_BASE_DIR}/definitions/include
        ${CMAKE_BASE_DIR}/include
        ${CMAKE_BASE_DIR}/utils/include
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

set_target_properties(instance_random PROPERTIES LINKER_LANGUAGE CXX)
