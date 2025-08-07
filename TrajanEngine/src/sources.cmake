set(ENGINE_SOURCES
        ${ENGINE_SOURCES}
        # CORE
        src/core/engine.cpp
        src/core/logger.cpp
        src/core/window.cpp

        # OPENGL RENDERER
        src/opengl/opengl_renderer.cpp

        # SYSTEMS
        src/systems/render_system.cpp
        src/systems/render_system.hpp

        # CORE
        src/core/component.hpp
        src/core/component_array.hpp
        src/core/component_manager.hpp
        src/core/engine.hpp
        src/core/entity.hpp
        src/core/entity_manager.hpp
        src/core/i_logger.hpp
        src/core/asset_handle.hpp
        src/core/i_renderer.hpp
        src/core/log.hpp
        src/core/logger.hpp
        src/core/math.hpp
        src/core/mesh.hpp
        src/core/orchestrator.hpp
        src/core/shader.hpp
        src/core/system.hpp
        src/core/system_manager.hpp
        src/core/texture.hpp
        src/core/window.hpp
        src/core/uuid.hpp
        src/core/i_asset_manager.hpp
        src/core/asset_system.hpp
        src/core/mesh_manager.hpp
        src/core/shader_manager.hpp

        # COMPONENTS
        src/components/sprite.hpp
        src/components/transform_2d.hpp

        # OPENGL RENDERER
        src/opengl/opengl_renderer.hpp

        # LIBRARY EXPORTS
        src/trajan_engine.hpp
)