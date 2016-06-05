# Initialize CXXFLAGS for Linux, OS X and MinGW on Windows
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
    set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
    set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -Wno-reorder")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

# Compiler-specific C++11 activation.
if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.9 or greater.")
    endif ()
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
        message(FATAL_ERROR "${PROJECT_NAME} requires clang 3.3 or greater")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (${MSVC_VERSION} LESS 1900)
        message(
            # MSVC 1900 has the best C++11 support so far. So, warn if you try to
            # use anything else.
          FATAL_ERROR
          "This Project does not support compiling on Windows with "
          "MSVC ${CMAKE_CXX_COMPILER_ID}. Please use MSVC 14 or higher.")
    endif()
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()


set (MAIN_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/buffwriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/csvwriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imagewriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm_bw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm_col.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm_grey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalcruncher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalcrunchmulti.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalcrunchsingle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalzoom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/printer.cpp
)

set (MAIN_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/buffwriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/csvwriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imagewriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm_bw.h
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm_col.h
    ${CMAKE_CURRENT_SOURCE_DIR}/image_pnm_grey.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalcruncher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalcrunchmulti.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalcrunchsingle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalparams.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fractalzoom.h
    ${CMAKE_CURRENT_SOURCE_DIR}/printer.h
)

set (HEADER_LIB
    ${CMAKE_CURRENT_SOURCE_DIR}/ctpl_stl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cxxopts.hpp
)

# Add custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(Threads REQUIRED)
find_package(SFML 2.3 COMPONENTS graphics)

if (${SFML_FOUND})
    include_directories(${SFML_INCLUDE_DIR})
    set (HAVE_SFML ON)
    set (MAIN_SOURCE 
        ${MAIN_SOURCE} 
        ${CMAKE_CURRENT_SOURCE_DIR}/image_sfml.cpp
        )
    set (MAIN_HEADER 
        ${MAIN_SOURCE} 
        ${CMAKE_CURRENT_SOURCE_DIR}/image_sfml.h
        )
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in config.h)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(geomandel ${MAIN_HEADER} ${HEADER_LIB} ${MAIN_SOURCE})
target_link_libraries(geomandel ${CMAKE_THREAD_LIBS_INIT})
if (${SFML_FOUND})
    target_link_libraries(geomandel ${SFML_LIBRARIES})
endif()

# TODO: It would be better to use this with CMAKEs test framework
# check if we need to build unit test executable
if (${UNIT_TEST})
    add_subdirectory(test)
endif()
