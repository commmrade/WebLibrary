cmake_minimum_required(VERSION 3.10.0)
project(proj VERSION 0.1.0 LANGUAGES C CXX)



set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)


add_executable(proj main.cpp requests/client.cpp requests/request.cpp requests/response.cpp)


target_include_directories(proj PUBLIC ${CMAKE_SOURCE_DIR}/include)

