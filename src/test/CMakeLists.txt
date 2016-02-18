set(UNIT_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_commandline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_computation.cpp
)

set (MAIN_HEADER_TEST
    ${CMAKE_CURRENT_SOURCE_DIR}/../global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../main_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../mandelparams.h
)

set (MAIN_SOURCE_TEST
    ${CMAKE_CURRENT_SOURCE_DIR}/../mandelzoom.cpp
)

add_executable(geomandel_tests ${UNIT_SOURCE} ${MAIN_HEADER_TEST} ${MAIN_SOURCE_TEST})
# target_link_libraries(geomandel_tests ${CMAKE_THREAD_LIBS_INIT})
