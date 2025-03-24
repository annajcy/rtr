#pragma once

#include "engine/global/base.h"
#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/state/rhi_pipeline_state.h"
#include "engine/runtime/shader.h"
#include "engine/runtime/texture.h"
#include "glm/fwd.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    PHONG
};

class Instanced_Material_base {
protected:
    std::vector<glm::mat4> m_instance_transforms{};
public:
    Instanced_Material_base(const std::vector<glm::mat4>& instance_tranforms) : 
        m_instance_transforms(instance_tranforms) {}
    virtual ~Instanced_Material_base() = default;

    unsigned int instance_count() const { return m_instance_transforms.size(); }
    const std::vector<glm::mat4>& instance_transforms() const { return m_instance_transforms; }
    std::vector<glm::mat4>& instance_transforms() { return m_instance_transforms; }
};

class Material : public GUID , public ISet_shader_uniform {
public:
    
protected:
    Material_type m_type{};
    Pipeline_state m_pipeline_state{};

    std::unordered_map<std::string, Binded_texture> m_textures{};
    std::shared_ptr<Shader> m_shader{};

public:
    Material(
        Material_type type, 
        const Pipeline_state &pipeline_state, 
        const std::unordered_map<std::string, Binded_texture>& textures, 
        const std::shared_ptr<Shader>& shader) : 
        GUID(), 
        ISet_shader_uniform(), 
        m_type(type), 
        m_pipeline_state(pipeline_state), 
        m_textures(textures), 
        m_shader(shader) {}
    virtual ~Material() = default;

    const Pipeline_state& pipeline_state() const { return m_pipeline_state; }
    Pipeline_state& pipeline_state() { return m_pipeline_state; }
    const std::unordered_map<std::string, Binded_texture>& textures() const { return m_textures; }
    std::unordered_map<std::string, Binded_texture>& textures() { return m_textures; }
    const std::shared_ptr<Shader>& shader() const { return m_shader; }
    std::shared_ptr<Shader>& shader() { return m_shader; }

    bool is_translucent() const {
        return m_pipeline_state.blend_state.enable == true;
    }

    void update_pipeline_state(std::shared_ptr<RHI_device>& device) const {
        device->pipeline_state()->blend_state() = m_pipeline_state.blend_state;
        device->pipeline_state()->depth_state() = m_pipeline_state.depth_state;
        device->pipeline_state()->polygon_offset_state() = m_pipeline_state.polygon_offset_state;
        device->pipeline_state()->stencil_state() = m_pipeline_state.stencil_state;
        device->pipeline_state()->cull_state() = m_pipeline_state.cull_state;
        device->pipeline_state()->clear_state() = m_pipeline_state.clear_state;
    }

    void upload_uniform(std::shared_ptr<Shader>& shader) override {
        for (auto& [name, binded_texture] : m_textures) {
            auto &[id, _] = binded_texture;
            shader->add_uniform(
                name,
                std::make_shared<Uniform_entry<int>>(id)
            );
        }
    }

};

class Instanced_material : public Material, public Instanced_Material_base {
public:
    Instanced_material(
        Material_type type,
        const Pipeline_state& pipeline_state,
        const std::unordered_map<std::string, Binded_texture>& textures,
        const std::shared_ptr<Shader>& shader,
        const std::vector<glm::mat4>& instance_tranforms
    ) : Material(type, pipeline_state, textures, shader), Instanced_Material_base(instance_tranforms) {}
    virtual ~Instanced_material() = default;

    void upload_uniform(std::shared_ptr<Shader>& shader) override {
        Material::upload_uniform(shader);
        shader->add_uniform("instance_transforms", std::make_shared<Uniform_entry<std::vector<glm::mat4>>>(m_instance_transforms));
    }
    
};

}