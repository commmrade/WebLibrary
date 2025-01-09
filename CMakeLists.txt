cmake_minimum_required(VERSION 3.10.0)
project(proj VERSION 0.1.0 LANGUAGES C CXX)



set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
# todo for dif compilers



# Find all .cpp files in the 'requests' and 'server' directories
file(GLOB_RECURSE REQUESTS_SOURCES "requests/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "server/*.cpp")

# Add executable with all .cpp files
add_executable(proj main.cpp ${REQUESTS_SOURCES} ${SERVER_SOURCES})



target_include_directories(proj PUBLIC ${CMAKE_SOURCE_DIR}/include)


target_link_libraries(proj PRIVATE jsoncpp)