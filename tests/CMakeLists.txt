project(CMonkeyTests)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_SOURCE_DIR}/cmake/ParseAndAddCatchTests.cmake)

set(TEST_FILES 
    tests.cpp
    lexer.cpp)

add_executable(tests ${TEST_FILES})

target_link_libraries(tests CMonkeyLib ${CONAN_LIBS})
set(PARSE_CATCH_TESTS_VERBOSE ON)
ParseAndAddCatchTests(tests)