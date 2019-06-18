project(CMonkeyTests)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_SOURCE_DIR}/cmake/ParseAndAddCatchTests.cmake)

set(TEST_FILES 
    tests.cpp
    eval.cpp
    lexer.cpp
    parser.cpp)

set(MEMORY_TEST
    mem.cpp)

add_executable(${PROJECT_NAME} ${TEST_FILES})
add_executable(${PROJECT_NAME}_Mem ${MEMORY_TEST})

target_include_directories(${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME}_Mem
  PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} CMonkeyLib ${CONAN_LIBS})
target_link_libraries(${PROJECT_NAME}_Mem CMonkeyLib ${CONAN_LIBS})
set(PARSE_CATCH_TESTS_VERBOSE ON)
ParseAndAddCatchTests(${PROJECT_NAME})