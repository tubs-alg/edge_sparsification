# Activate concurrency?
option(ACTIVATE_CONCURRENCY
        "Enable concurrency"
        OFF)
#set(ACTIVATE_CONCURRENCY 1)
if( ACTIVATE_CONCURRENCY )
    message("Searching fo TBB")
    find_package( TBB REQUIRED )
    if( TBB_FOUND )
        include(${TBB_USE_FILE})
        message("Found TBB")
    endif()
endif()

add_library(mwt_library
        src/mwt_library/mwt_library.cpp
        )

target_include_directories(mwt_library
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${MWT_ROOT_DIR}
        )

target_link_libraries(mwt_library
        algutil::algcplex CGAL gmp mpfr ${TBB_LIBRARIES}
        )