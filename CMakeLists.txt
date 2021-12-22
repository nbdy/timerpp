cmake_minimum_required(VERSION 3.4)
project(timerpp)

option(TESTS "Build tests" ON)
option(PYTHON "Build python binding" ON)

set(CMAKE_CXX_STANDARD 11)

if(TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()

    add_executable(timerpp_tests tests.cpp)
    target_compile_definitions(timerpp_tests PRIVATE -DTESTS)
    target_link_libraries(timerpp_tests dl gtest_main)
    include(GoogleTest)
    gtest_discover_tests(timerpp_tests)
endif()

if(PYTHON)
    find_package(pybind11 CONFIG REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    pybind11_add_module(timerpp_py python_binding.cpp)
endif()

add_executable(timerpp_example main.cpp timerpp.h)
target_link_libraries(timerpp_example pthread)