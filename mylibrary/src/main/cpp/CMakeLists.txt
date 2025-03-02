cmake_minimum_required(VERSION 3.22.1)

project("hivefxso")

########################## 设置 c++ 20 开始 ################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

########################## 设置 c++ 20 结束 ################################

########################## 第三方库 相关配置  开始 ################################

include_directories(${CMAKE_SOURCE_DIR}/opencv-mobile/sdk/native/jni/include)

set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv-mobile/sdk/native/jni")

########################## 第三方库 相关配置  结束 ################################

add_library(${CMAKE_PROJECT_NAME} SHARED
        SlideWindow.cpp
        SingleTexturePlayer.cpp
        SequenceFramePlayer.cpp
        BillBoardManager.cpp
        AsyncSequenceFramePlayer.cpp
        ShaderProgram.cpp
        ScreenQuad.cpp
        Texture2D.cpp
        JsonReader.cpp
        EnumReflect.cpp
        SnowStylizer.cpp
        TextureBlender.cpp
        TimeUtils.cpp)

set(REQUIRED_PACKAGES
        glm
        WebP
        jsoncpp
        OpenCV
)

foreach(PACKAGE ${REQUIRED_PACKAGES})
    find_package(${PACKAGE} REQUIRED)
endforeach()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        EGL
        GLESv3
        glm::glm
        libwebp::libwebp
        JsonCpp::JsonCpp
        ${OpenCV_LIBS}
)

# 设置库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})