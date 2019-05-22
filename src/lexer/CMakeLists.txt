project(CMonkeyLexer) 

add_library(${PROJECT_NAME} 
	lexer.cpp) 

target_include_directories(${PROJECT_NAME}
	PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} 
  PUBLIC coverage_config CMonkeyCommon)