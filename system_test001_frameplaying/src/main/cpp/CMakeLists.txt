cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(${CMAKE_SOURCE_DIR}/../../../../mylibrary/src/main/cpp)

project("system_test001_frameplaying")

add_library(${CMAKE_PROJECT_NAME} SHARED
    system_test001_frameplaying.cpp
    AndroidOut.cpp
    Renderer.cpp
    Shader.cpp
    TextureAsset.cpp
    Utility.cpp)

find_package(game-activity REQUIRED CONFIG)

target_link_libraries(${CMAKE_PROJECT_NAME}
    game-activity::game-activity
    EGL
    GLESv3
    jnigraphics
    android
    log)
