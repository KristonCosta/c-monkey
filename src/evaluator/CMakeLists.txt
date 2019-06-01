project(CMonkeyEvaluator) 

add_library(${PROJECT_NAME}  
  bag.cpp
  builtin.cpp
  env.cpp
	eval.cpp) 

target_include_directories(${PROJECT_NAME}
	PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} 
  PUBLIC coverage_config CMonkeyToken CMonkeyParser)