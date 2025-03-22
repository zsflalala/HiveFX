cmake_minimum_required(VERSION 3.22.1)

project("system_test008_buildsimplestapk")

set(REQUIRED_PACKAGES
        game-activity
        glm
        WebP
        jsoncpp
)

foreach(PACKAGE ${REQUIRED_PACKAGES})
    find_package(${PACKAGE} REQUIRED)
endforeach()

add_library(BasicLibrary SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/SingleTexturePlayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/ShaderProgram.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/ScreenQuad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/Texture2D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/JsonReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/EnumReflect.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/TimeUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/AppContext.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/FileUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/ThreadPool.cpp
)

target_include_directories(BasicLibrary PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary
)

target_link_libraries(BasicLibrary PUBLIC
        game-activity::game-activity
        EGL
        GLESv3
        jnigraphics
        android
        log
        glm::glm
        libwebp::libwebp
        JsonCpp::JsonCpp
)

add_library(${CMAKE_PROJECT_NAME} SHARED
        system_test008_buildsimplestapk.cpp
        Renderer.cpp
        Renderers/WeatherAPKRenderer.cpp
        Renderers/BackGroundAPKRenderer.cpp
        Renderers/WeatherSeqRenderer.cpp
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        BasicLibrary)