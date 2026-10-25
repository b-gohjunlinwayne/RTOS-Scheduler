cmake_minimum_required(VERSION 3.20)

cmake_policy(SET CMP0048 NEW)

project(RTOSScheduler
    VERSION 0.1.0
    DESCRIPTION "real time scheduling simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

add_subdirectory(Extern/glfw)

add_library(SchedulerSimulator STATIC
    src/Schedulers/EDFScheduler.cpp 
    src/Schedulers/RMSScheduler.cpp 
    src/Schedulers/FPPSScheduler.cpp
    src/Schedulers/Scheduler.cpp
    src/Simulator.cpp)

target_include_directories(SchedulerSimulator
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# 1. Define the ImGui static library
add_library(imgui STATIC
    Extern/imgui/imgui.cpp
    Extern/imgui/imgui_demo.cpp
    Extern/imgui/imgui_draw.cpp
    Extern/imgui/imgui_tables.cpp
    Extern/imgui/imgui_widgets.cpp
    Extern/imgui/backends/imgui_impl_glfw.cpp
    Extern/imgui/backends/imgui_impl_opengl3.cpp
)

# 2. FIX: Tell the 'imgui' target where to find its own headers (imgui.h, etc.)
target_include_directories(imgui 
    PUBLIC
        Extern/imgui
        Extern/imgui/backends
)

target_link_libraries(imgui 
    PUBLIC 
        glfw
        OpenGL::GL
)

add_executable(
    RTOSScheduler
    src/main.cpp
    src/Editor.cpp)

target_include_directories(RTOSScheduler
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/Extern/imgui
        ${CMAKE_SOURCE_DIR}/Extern/imgui/backends
)

target_link_libraries(RTOSScheduler PRIVATE 
    SchedulerSimulator
    imgui)

if(MSVC)
    target_compile_options(RTOSScheduler PRIVATE /W4)
else()
    target_compile_options(RTOSScheduler PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

include(CTest)
option(BUILD_TESTING "Build the testing tree." ON)

if(BUILD_TESTING)
    add_executable(
        RTOSScheduler_tests
        testsrc/test_main.cpp
        testsrc/SchedulerTest/testScheduler.cpp 
        testsrc/TestSimulator.cpp)
    
    target_include_directories(RTOSScheduler_tests
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/TestInclude
    )

    target_link_libraries(RTOSScheduler_tests PRIVATE SchedulerSimulator)

    add_test(
        NAME RTOSScheduler.BasicTests
        COMMAND RTOSScheduler_tests
    )

endif()