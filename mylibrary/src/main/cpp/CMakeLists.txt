cmake_minimum_required(VERSION 3.22.1)

project("hivefxso")

########################## 设置 c++ 20 开始 ################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
########################## 设置 c++ 20 结束 ################################

########################## 第三方库 相关配置 开始 ##########################

# OpenCV
include_directories(${CMAKE_SOURCE_DIR}/opencv-mobile/sdk/native/jni/include)
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv-mobile/sdk/native/jni")

# KTX 路径设置
if(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    set(KTX_LIB_PATH ${CMAKE_SOURCE_DIR}/KTX-4.4.0/arm64-v8a/lib)
    set(KTX_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/KTX-4.4.0/arm64-v8a/include)
elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
    set(KTX_LIB_PATH ${CMAKE_SOURCE_DIR}/KTX-4.4.0/x86_64/lib)
    set(KTX_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/KTX-4.4.0/x86_64/include)
else()
    message(FATAL_ERROR "Unsupported ABI: ${CMAKE_ANDROID_ARCH_ABI}")
endif()

include_directories(${KTX_INCLUDE_PATH})

# KTX
add_library(ktx SHARED IMPORTED)
set_target_properties(ktx PROPERTIES IMPORTED_LOCATION ${KTX_LIB_PATH}/libktx.so)

########################## 第三方库 相关配置 结束 ##########################

add_library(${CMAKE_PROJECT_NAME} SHARED
        lodepng.cpp
        SlideWindow.cpp
        SingleTexturePlayer.cpp
        SequenceFramePlayer.cpp
        BillBoardManager.cpp
        SplashManager.cpp
        AsyncSequenceFramePlayer.cpp
        ShaderProgram.cpp
        ScreenQuad.cpp
        Texture2D.cpp
        JsonReader.cpp
        EnumReflect.cpp
        SnowStylizer.cpp
        TextureBlender.cpp
        BlendManager.cpp
        TimeUtils.cpp
        AppContext.cpp
        FileUtils.cpp
        ThreadPool.cpp
        EffectTester.cpp
)

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
        ktx
)

# 设置库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
