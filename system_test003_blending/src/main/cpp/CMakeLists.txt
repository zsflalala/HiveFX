cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_VERBOSE_MAKEFILE ON)

project("system_test003_blending")

########################## 第三方库 相关配置  开始################################

add_library(mylibrary SHARED IMPORTED)

set_target_properties(mylibrary PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../../../../mylibrary/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libhivefxso.so)

include_directories(${CMAKE_SOURCE_DIR}/../../../../mylibrary/src/main/cpp)

########################## 第三方库 相关配置  结束################################

add_library(${CMAKE_PROJECT_NAME} SHARED
    system_test003_blending.cpp
    Renderer.cpp
    Renderers/FullScreenSequenceBlendRenderer.cpp
)

find_package(game-activity REQUIRED CONFIG)

set(REQUIRED_PACKAGES
        glm
        WebP
)

foreach(PACKAGE ${REQUIRED_PACKAGES})
    find_package(${PACKAGE} REQUIRED)
endforeach()

target_link_libraries(${CMAKE_PROJECT_NAME}
    game-activity::game-activity
    EGL
    GLESv3
    jnigraphics
    android
    log
    mylibrary
    glm::glm
    libwebp::libwebp)
