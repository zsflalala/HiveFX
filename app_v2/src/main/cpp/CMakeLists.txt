cmake_minimum_required(VERSION 3.22.1)

project("app_v2")

########################## 第三方库 相关配置  开始################################

add_library(hivefxso SHARED IMPORTED)

set_target_properties(hivefxso PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../../../../mylibrary/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libhivefxso.so)

include_directories(${CMAKE_SOURCE_DIR}/../../../../mylibrary/src/main/cpp)

########################## 第三方库 相关配置  结束################################

add_library(${CMAKE_PROJECT_NAME} SHARED
        Renderer.cpp
        app_v2.cpp
        Renderers/SnowQuantizationBicPicRenderer.cpp
        Renderers/SnowQuantizationSeqRenderer.cpp
        Renderers/RainQuantizationBicPicRenderer.cpp
        Renderers/RainQuantizationSeqRenderer.cpp
        Renderers/ScrollRainRenderer.cpp
)

set(REQUIRED_PACKAGES
        game-activity
        glm
        WebP
        jsoncpp
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
        hivefxso
        glm::glm
        libwebp::libwebp
        JsonCpp::JsonCpp)