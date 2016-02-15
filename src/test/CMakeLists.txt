set(UNIT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_commandline.cpp
)

add_executable(geomandel_tests ${UNIT_SOURCES})
