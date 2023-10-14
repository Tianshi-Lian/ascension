target_sources(${APP_NAME} PRIVATE
    ascension.cpp
    main.cpp

    # Assets
    assets/asset_manager.cpp

    # Core
    core/application.cpp
    core/window.cpp

    # Debug
    debug/instrumentor.cpp
    debug/logger.cpp

    # Platform
    platform/platform_win32.cpp

    # Input
    input/input.cpp
    input/input_listener.cpp
    input/input_manager.cpp
    input/input_state.cpp

    # Graphics
    graphics/buffer_object.cpp
    graphics/frame_buffer.cpp
    graphics/renderer_2d.cpp
    graphics/shader.cpp
    graphics/sprite_batch.cpp
    graphics/sprite_font.cpp
    graphics/texture_2d.cpp
    graphics/texture_atlas.cpp
    graphics/vertex_array_object.cpp
)
