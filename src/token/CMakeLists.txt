project(CMonkeyToken) 

add_library(${PROJECT_NAME} 
	token.cpp) 

target_include_directories(${PROJECT_NAME}
	PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} 
  PUBLIC coverage_config)
