project(CMonkeyLib)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common) 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lexer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/parser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/evaluator)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/repl)


add_library(${PROJECT_NAME} lib.cpp)

target_include_directories(${PROJECT_NAME} 
  PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC coverage_config CMonkeyCommon CMonkeyLexer CMonkeyParser CMonkeyEvaluator CMonkeyRepl)

