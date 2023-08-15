target_sources(${APP_NAME} PRIVATE
    ascension.hpp

    # Assets
    assets/asset_manager.hpp
    assets/asset_types.hpp
    assets/texture_handler.hpp

    # Core
    core/application.hpp
    core/types.hpp
    core/window.hpp

    # Input
    input/input_listener.hpp
    input/input_state.hpp
    input/input_types.hpp

    # Graphics
    graphics/buffer_object.hpp
    graphics/frame_buffer.hpp
    graphics/renderer_2d.hpp
    graphics/shader_data_types.hpp
    graphics/shader.hpp
    graphics/sprite_batch.hpp
    graphics/sprite_font.hpp
    graphics/texture_2d.hpp
    graphics/texture_atlas.hpp
    graphics/vertex_array_object.hpp
)
