cmake_minimum_required(VERSION 3.11)
project(CNNFeatureExtraction)
include(CMakeFindDependencyMacro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options(-fPIC)
add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

## Dependencies
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Eigen3 3.3.3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR}
                    ${EMP-OT_INCLUDE_DIRS} 
                    ${EMP-TOOL_INCLUDE_DIRS})


include_directories(include)
add_subdirectory(include)

## SCI
add_subdirectory(main)
include_directories(main/src/)

## demo
macro (add_network_cheetah _name)
  add_executable(${_name}-cheetah "networks/main_${_name}.cpp")
  target_link_libraries(${_name}-cheetah gemini SCI-Cheetah)
  target_compile_definitions(${_name}-cheetah PUBLIC SCI_OT=1 USE_CHEETAH=1)
endmacro()
