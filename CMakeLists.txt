cmake_minimum_required(VERSION 3.15)
project(weblib VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(BUILD_SHARED_LIBS "Build shared library" ON)

# Dependencies
include(FetchContent)
include_directories(include)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)


# Library
add_library(weblib
    requests/client.cpp
    requests/request.cpp
    requests/response.cpp

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

if(JSONCPP_FOUND)
    message(STATUS "jsoncpp include dirs: ${JSONCPP_INCLUDE_DIRS}")
    message(STATUS "jsoncpp libs: ${JSONCPP_LIBRARIES}")

    target_include_directories(weblib PRIVATE ${JSONCPP_INCLUDE_DIRS})
    target_link_libraries(weblib PRIVATE ${JSONCPP_LIBRARIES})
else()
    message(FATAL_ERROR "jsoncpp not found via pkg-config")
endif()


target_include_directories(weblib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
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

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
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
# enable_testing()
# if(TEST_SOURCES)
#     add_executable(tests ${TEST_SOURCES})
#     target_link_libraries(tests PRIVATE weblib gtest_main ${JSONCPP_LIBRARIES})
#     target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
#     include(GoogleTest)
#     gtest_discover_tests(tests)
# else()
#     message(WARNING "No tests will be built due to missing test sources")
# endif()
