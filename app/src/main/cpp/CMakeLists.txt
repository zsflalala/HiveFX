cmake_minimum_required(VERSION 3.22.1)

project("hivefx")

add_library(hivefx SHARED
        main.cpp
        Renderer.cpp
        Renderers/SnowRenderer.cpp
        Renderers/CloudRendererBillBoard.cpp
        Renderers/RainRendererAsync.cpp
        Renderers/SnowSceneRendererAsync.cpp
        SingleTexturePlayer.cpp
        SequenceFramePlayer.cpp
        BillBoardManager.cpp
        AsyncSequenceFramePlayer.cpp
        ShaderProgram.cpp
        ScreenQuad.cpp
        Texture2D.cpp
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
        glm::glm
        libwebp::libwebp
    )