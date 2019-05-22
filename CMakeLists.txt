cmake_minimum_required(VERSION 3.11)
project(CMonkey)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(coverage_config INTERFACE)

set(SOURCE_FILES  src/main.cpp )

add_executable(main ${SOURCE_FILES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(main 
  CMonkeyLib ${LIBRARIES} ${CONAN_LIBS}) 

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(coverage_config INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(coverage_config INTERFACE --coverage)
  else()
    target_link_libraries(coverage_config INTERFACE --coverage)
  endif()
endif(CODE_COVERAGE)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

