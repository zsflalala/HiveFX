cmake_minimum_required(VERSION 3.22.1)

project("system_test008_buildsimplestapk")

set(REQUIRED_PACKAGES
        game-activity
        glm
        jsoncpp
)

foreach(PACKAGE ${REQUIRED_PACKAGES})
    find_package(${PACKAGE} REQUIRED)
endforeach()

add_library(BasicLibrary SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/SingleTexturePlayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/SequenceFramePlayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/ShaderProgram.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/ScreenQuad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/Texture2D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/JsonReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/EnumReflect.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/TimeUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/AppContext.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicLibrary/FileUtils.cpp
)

# KTX 路径设置
if(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    set(KTX_LIB_PATH ${CMAKE_SOURCE_DIR}/BasicLibrary/KTX-4.4.0/arm64-v8a/lib)
    set(KTX_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/BasicLibrary/KTX-4.4.0/arm64-v8a/include)
elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
    set(KTX_LIB_PATH ${CMAKE_SOURCE_DIR}/BasicLibrary/KTX-4.4.0/x86_64/lib)
    set(KTX_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/BasicLibrary/KTX-4.4.0/x86_64/include)
else()
    message(FATAL_ERROR "Unsupported ABI: ${CMAKE_ANDROID_ARCH_ABI}")
endif()

include_directories(${KTX_INCLUDE_PATH})

# KTX
add_library(ktx SHARED IMPORTED)
set_target_properties(ktx PROPERTIES IMPORTED_LOCATION ${KTX_LIB_PATH}/libktx.so)

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
        JsonCpp::JsonCpp
        ktx
)

add_library(${CMAKE_PROJECT_NAME} SHARED
        system_test008_buildsimplestapk.cpp
        Renderer.cpp
        Renderers/RainMultiChannelSeqRenderer.cpp
        Renderers/NightSceneSequencePlayer.cpp
        Renderers/LightningRenderer.cpp
        Renderers/LightningSequencePlayer.cpp
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        BasicLibrary)