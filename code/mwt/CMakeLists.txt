if(GCC)
    add_compile_options(-frounding-math)
endif()

set(MWT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mwt/include")

add_subdirectory(mwt_library)