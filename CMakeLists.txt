cmake_minimum_required(VERSION 3.11)
project(CMonkey)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(SOURCE_FILES  src/main.cpp )

add_executable(main ${SOURCE_FILES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(main 
  CMonkeyLib ${LIBRARIES} ${CONAN_LIBS}) 

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

