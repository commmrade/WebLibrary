cmake_minimum_required(VERSION 3.28)
project(weblib VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(BUILD_SHARED_LIBS "Build shared library" ON)

# Dependencies
include(FetchContent)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Find jsoncpp with fallback to pkg-config
find_package(jsoncpp CONFIG QUIET)
if(jsoncpp_FOUND)
    message(STATUS "Found jsoncpp via find_package")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(JSONCPP jsoncpp REQUIRED)
    message(STATUS "Found jsoncpp via pkg-config")
endif()

# Library
add_library(weblib
    server/Cookie.cpp
    server/HttpHandle.cpp
    server/HttpRequest.cpp
    server/HttpResponse.cpp
    server/HttpRouter.cpp
    server/HttpServer.cpp
    server/Utils.cpp
    server/HttpQuery.cpp
    server/HttpHeaders.cpp
)

if(jsoncpp_FOUND)
    target_link_libraries(weblib PRIVATE jsoncpp_lib)
elseif(JSONCPP_FOUND)
    target_include_directories(weblib PRIVATE ${JSONCPP_INCLUDE_DIRS})
    target_link_libraries(weblib PRIVATE ${JSONCPP_LIBRARIES})
endif()

target_include_directories(weblib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/weblib>
)

target_compile_options(weblib PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_compile_definitions(weblib PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

set_target_properties(weblib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME weblib
    OUTPUT_NAME weblib
)

# Installation
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_INSTALL_INCLUDEDIR "include/weblib" CACHE PATH "")
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(TARGETS weblib
        EXPORT weblibTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT weblibTargets
        FILE weblibTargets.cmake
        NAMESPACE weblib::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/weblib)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/weblibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/weblibConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/weblib
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/weblibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/weblibConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/weblibConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/weblib
)

# Tests
enable_testing()
file(GLOB TEST_SOURCES tests/*.cpp)
if(TEST_SOURCES)
    add_executable(tests ${TEST_SOURCES})
    target_link_libraries(tests PRIVATE weblib GTest::gtest_main)
    if(JSONCPP_FOUND)
        target_include_directories(tests PRIVATE ${JSONCPP_INCLUDE_DIRS})
        target_link_libraries(tests PRIVATE ${JSONCPP_LIBRARIES})
    elseif(jsoncpp_FOUND)
        target_link_libraries(tests PRIVATE jsoncpp_lib)
    endif()
    target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    include(GoogleTest)
    gtest_discover_tests(tests)
else()
    message(WARNING "No tests found in tests/ directory")
endif()

# clang tidy stuff
find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if(CLANG_TIDY_EXECUTABLE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXECUTABLE}")
    # Collect all source and header files for clang-tidy using non-recursive GLOB for compatibility
    set(CLANG_TIDY_SOURCES)
    # C++ sources from known directories
    file(GLOB _sources "${CMAKE_CURRENT_SOURCE_DIR}/requests/*.cpp")
    list(APPEND CLANG_TIDY_SOURCES ${_sources})
    file(GLOB _sources "${CMAKE_CURRENT_SOURCE_DIR}/server/*.cpp")
    list(APPEND CLANG_TIDY_SOURCES ${_sources})
    file(GLOB _sources "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
    list(APPEND CLANG_TIDY_SOURCES ${_sources})
    # Headers from include (up to one sub-level for common structures like include/weblib/*.h)
    file(GLOB _headers "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
    list(APPEND CLANG_TIDY_SOURCES ${_headers})
    file(GLOB _headers "${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.h")
    list(APPEND CLANG_TIDY_SOURCES ${_headers})
    # Create custom target for clang-tidy
    add_custom_target(clang-tidy
        COMMAND ${CLANG_TIDY_EXECUTABLE}
            -p=${CMAKE_BINARY_DIR}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
            ${CLANG_TIDY_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy on source files"
        VERBATIM
    )
else()
    message(WARNING "clang-tidy not found. Skipping clang-tidy target.")
endif()