cmake_minimum_required(VERSION 3.5.0)
project(proj VERSION 0.1.0 LANGUAGES C CXX)



set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(Boost REQUIRED)

add_executable(proj main.cpp)

target_link_libraries(proj PRIVATE Boost::headers)

