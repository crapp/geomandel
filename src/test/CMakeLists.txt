set(UNIT_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/buffwriter_mock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mandelcruncher_mock.h
)

set(UNIT_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_commandline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_computation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_output.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffwriter_mock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mandelcruncher_mock.cpp
)

set (MAIN_HEADER_TEST
    ${CMAKE_CURRENT_SOURCE_DIR}/../buffwriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../main_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../mandelcruncher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../fractalparams.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../fractalzoom.h
)

set (MAIN_SOURCE_TEST
    ${CMAKE_CURRENT_SOURCE_DIR}/../buffwriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../mandelcruncher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../fractalzoom.cpp
)

add_executable(geomandel_tests
    ${UNIT_HEADER}
    ${UNIT_SOURCE}
    ${MAIN_HEADER_TEST}
    ${MAIN_SOURCE_TEST})
# target_link_libraries(geomandel_tests ${CMAKE_THREAD_LIBS_INIT})
