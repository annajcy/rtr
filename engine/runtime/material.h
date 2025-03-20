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
    PHONG
};

class Material : public GUID , public ISet_shader_uniform {
protected:
    Material_type m_type{};
    Pipeline_state m_pipeline_state{};
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures{};

public:
    Material(Material_type type) : GUID(), ISet_shader_uniform(), m_type(type) {}
    virtual ~Material() = default;

    const Pipeline_state& pipeline_state() const { return m_pipeline_state; }
    const std::unordered_map<std::string, std::shared_ptr<Texture>>& textures() const { return m_textures; }
    std::shared_ptr<Texture> texture(const std::string& name) const { return m_textures.at(name); }
    void set_texture(const std::string& name, std::shared_ptr<Texture> texture) { m_textures[name] = texture; }

};


class Phong_material : public Material {
protected:
    glm::vec3 m_ambient{};
    glm::vec3 m_diffuse{};
    glm::vec3 m_specular{};
    float m_shininess{};
public:
    Phong_material() : Material(Material_type::PHONG), m_ambient(glm::one<glm::vec3>()), m_diffuse(glm::one<glm::vec3>()), m_specular(glm::one<glm::vec3>()), m_shininess(1.0f) {}

    ~Phong_material() = default;
    const glm::vec3& ambient() const { return m_ambient; }
    const glm::vec3& diffuse() const { return m_diffuse; }
    const glm::vec3& specular() const { return m_specular; }
    float shininess() const { return m_shininess; }
    
    glm::vec3& ambient() { return m_ambient; }
    glm::vec3& diffuse() { return m_diffuse; }
    glm::vec3& specular() { return m_specular; }
    float& shininess() { return m_shininess; }

    void upload_uniform(std::shared_ptr<Shader>& shader) override {
        shader->add_uniform("material.ambient", std::make_shared<Uniform_entry<glm::vec3>>(m_ambient));
        shader->add_uniform("material.diffuse", std::make_shared<Uniform_entry<glm::vec3>>(m_diffuse));
        shader->add_uniform("material.specular", std::make_shared<Uniform_entry<glm::vec3>>(m_specular));
        shader->add_uniform("material.shininess", std::make_shared<Uniform_entry<float>>(m_shininess));
    }
};

}