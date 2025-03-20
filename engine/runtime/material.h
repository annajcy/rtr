#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/state/rhi_pipeline_state.h"
#include "engine/runtime/shader.h"
#include "engine/runtime/texture.h"

namespace rtr {

struct Pipeline_state {
    Depth_state depth_state{};
    Blend_state blend_state{};
    Polygon_offset_state polygon_offset_state{};
    Stencil_state stencil_state{};
    Cull_state cull_state{};
    Clear_state clear_state{};

    static Pipeline_state opaque_pipeline_state() {
        return Pipeline_state{
            Depth_state::opaque(),
            Blend_state::opaque(),
            Polygon_offset_state::disabled(),
            Stencil_state::opaque(),
            Cull_state::back(),
            Clear_state::enabled()
        };
    }

    static Pipeline_state translucent_pipeline_state() {
        return Pipeline_state{
            Depth_state::translucent(),
            Blend_state::translucent(),
            Polygon_offset_state::disabled(),
            Stencil_state::disabled(),
            Cull_state::back(),
            Clear_state::enabled()
        };
    }

    static Pipeline_state edge_pipeline_state() {
        return Pipeline_state{
            Depth_state::opaque(),
            Blend_state::opaque(),
            Polygon_offset_state::disabled(),
            Stencil_state::edge(),
            Cull_state::back(),
            Clear_state::enabled()
        };
    }
};

enum class Material_type {
    WHITE,
    DEPTH,
    EDGE,
    TRANSLUCENT_PHONG,
    OPAUE_PHONG,
};

class Material : public GUID , public ISet_shader_uniform {
protected:
    Pipeline_state m_pipeline_state{};
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures{};

public:
    Material() : GUID(), ISet_shader_uniform(), m_pipeline_state(Pipeline_state::opaque_pipeline_state()) {}
    Material(const Pipeline_state& pipeline_state, const std::unordered_map<std::string, std::shared_ptr<Texture>>& textures) : GUID(), m_pipeline_state(pipeline_state), m_textures(textures) {}
    virtual ~Material() = default;

    const Pipeline_state& pipeline_state() const { return m_pipeline_state; }
    const std::unordered_map<std::string, std::shared_ptr<Texture>>& textures() const { return m_textures; }
    std::shared_ptr<Texture> texture(const std::string& name) const { return m_textures.at(name); }
    void set_texture(const std::string& name, std::shared_ptr<Texture> texture) { m_textures[name] = texture; }

};

}