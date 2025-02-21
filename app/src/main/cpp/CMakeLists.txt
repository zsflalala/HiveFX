cmake_minimum_required(VERSION 3.22.1)

project("hivefx")

########################## 第三方库 相关配置  开始################################

add_library(mylibrary SHARED IMPORTED)

set_target_properties(mylibrary PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libmylibrary.so)

include_directories(${PROJECT_SOURCE_DIR}/third_party/Players/include)

########################## 第三方库 相关配置  结束################################

add_library(hivefx SHARED
        main.cpp
        Renderer.cpp
        Renderers/SnowRenderer.cpp
        Renderers/CloudRendererBillBoard.cpp
        Renderers/RainRendererAsync.cpp
        Renderers/SnowSceneRenderer.cpp
        Renderers/RainRenderer.cpp
)

find_package(game-activity REQUIRED CONFIG)

set(REQUIRED_PACKAGES
        glm
        WebP
)

foreach(PACKAGE ${REQUIRED_PACKAGES})
    find_package(${PACKAGE} REQUIRED)
endforeach()

target_link_libraries(hivefx
        game-activity::game-activity
        EGL
        GLESv3
        jnigraphics
        android
        log
        mylibrary   # 链接第三方库
        glm::glm
        libwebp::libwebp
    )