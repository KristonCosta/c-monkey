project(CMonkeyTests)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_SOURCE_DIR}/cmake/ParseAndAddCatchTests.cmake)

set(TEST_FILES 
    tests.cpp
    eval.cpp
    lexer.cpp
    parser.cpp)

add_executable(${PROJECT_NAME} ${TEST_FILES})

target_include_directories(${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} CMonkeyLib ${CONAN_LIBS})
set(PARSE_CATCH_TESTS_VERBOSE ON)
ParseAndAddCatchTests(${PROJECT_NAME})