cmake_minimum_required(VERSION 3.11)
project(CMonkey)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(SOURCE_FILES  src/main.cpp )

add_executable(main ${SOURCE_FILES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(main 
  CMonkeyLib ${LIBRARIES} ${CONAN_LIBS}) 

