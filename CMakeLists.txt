cmake_minimum_required(VERSION 3.15)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(TBB REQUIRED)

file(GLOB SRCS CMAKE_CONFIGURE_DEPENDS source/*.cpp)
add_executable(Compile ${SRCS})
target_link_libraries(Compile PRIVATE TBB::tbb)