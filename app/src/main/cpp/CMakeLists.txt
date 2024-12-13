cmake_minimum_required(VERSION 3.22.1)

project("hivefx")

add_library(hivefx SHARED
        main.cpp
        Renderer.cpp
        SequenceFrameRenderer.cpp
        TextureAsset.cpp
        stb_init.cpp
    )

find_package(game-activity REQUIRED CONFIG)

target_link_libraries(hivefx
        game-activity::game-activity
        EGL
        GLESv3
        jnigraphics
        android
        log
    )