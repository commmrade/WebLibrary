cmake_minimum_required(VERSION 3.10.0)
project(proj VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -DDEBUG")

# Источники проекта
file(GLOB_RECURSE REQUESTS_SOURCES "requests/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "server/*.cpp")

add_executable(proj main.cpp ${REQUESTS_SOURCES} ${SERVER_SOURCES})
target_include_directories(proj PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(proj PRIVATE jsoncpp)

# ===== GoogleTest Setup =====
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Для CMake >= 3.14
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ===== Тесты =====
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

add_executable(tests ${TEST_SOURCES} ${SERVER_SOURCES})
target_link_libraries(tests PRIVATE gtest_main jsoncpp)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME AllTests COMMAND tests)
