cmake_minimum_required(VERSION 3.22.1)

project("hivefxso")

add_library(${CMAKE_PROJECT_NAME} SHARED
        SingleTexturePlayer.cpp
        SequenceFramePlayer.cpp
        BillBoardManager.cpp
        AsyncSequenceFramePlayer.cpp
        ShaderProgram.cpp
        ScreenQuad.cpp
        Texture2D.cpp
        JsonReader.cpp
        TextureBlender.cpp)

set(REQUIRED_PACKAGES
        glm
        WebP
        jsoncpp
)

foreach(PACKAGE ${REQUIRED_PACKAGES})
    find_package(${PACKAGE} REQUIRED)
endforeach()

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        EGL
        GLESv3
        glm::glm
        libwebp::libwebp
        JsonCpp::JsonCpp
)

# 设置库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})