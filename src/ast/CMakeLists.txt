project(CMonkeyAST) 

add_library(${PROJECT_NAME} 
  ast.cpp) 

target_include_directories(${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} 
  PUBLIC coverage_config CMonkeyToken)