#pragma once

#include "engine/global/base.h"
#include "engine/global/guid.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"

#include "engine/runtime/core/camera.h"
#include "engine/runtime/core/node.h"
#include "engine/runtime/core/light.h"
#include "engine/runtime/core/shader.h"
#include "engine/runtime/core/texture.h"

namespace rtr {

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

class Material : public GUID {
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


    virtual void upload_uniform(const std::shared_ptr<Node>& node, const std::shared_ptr<Camera_setting>& camera_setting, const std::shared_ptr<Light_setting>& light_setting) {
        for (auto& [name, texture] : m_textures) {
            m_shader->add_uniform(name, std::make_shared<Uniform_entry<int>>((int)texture.first));
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

    virtual void upload_uniform(const std::shared_ptr<Node>& node, const std::shared_ptr<Camera_setting>& camera_setting, const std::shared_ptr<Light_setting>& light_setting) {
        Material::upload_uniform(node, camera_setting, light_setting);
        m_shader->add_uniform("instance_transforms", std::make_shared<Uniform_entry<std::vector<glm::mat4>>>(m_instance_transforms));
    }
    
};

class Phong_material_base {
protected:
    glm::vec3 m_ambient{};
    glm::vec3 m_diffuse{};
    glm::vec3 m_specular{};
    float m_shininess{};
public:
    Phong_material_base(
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float shininess
    ) : m_ambient(ambient),
        m_diffuse(diffuse),
        m_specular(specular),
        m_shininess(shininess) {}

    ~Phong_material_base() = default;
    const glm::vec3& ambient() const { return m_ambient; }
    const glm::vec3& diffuse() const { return m_diffuse; }
    const glm::vec3& specular() const { return m_specular; }
    float shininess() const { return m_shininess; }
    glm::vec3& ambient() { return m_ambient; }
    glm::vec3& diffuse() { return m_diffuse; }
    glm::vec3& specular() { return m_specular; }
    float& shininess() { return m_shininess; }
};


class Phong_material : public Material, public Phong_material_base {

public:
    Phong_material(
        const Pipeline_state &pipeline_state,
        const std::unordered_map<std::string, Binded_texture>& textures,
        const std::shared_ptr<Shader>& shader,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float shininess
    ) : Material (
        Material_type::PHONG,
        pipeline_state,
        textures,
        shader
    ) , Phong_material_base(
        ambient,
        diffuse,
        specular,
        shininess
    ) {}

    virtual ~Phong_material() = default;

    virtual void upload_uniform(const std::shared_ptr<Node>& node, const std::shared_ptr<Camera_setting>& camera_setting, const std::shared_ptr<Light_setting>& light_setting) override {
        Material::upload_uniform(node, camera_setting, light_setting);
        m_shader->add_uniform("material.ambient", std::make_shared<Uniform_entry<glm::vec3>>(m_ambient));
        m_shader->add_uniform("material.diffuse", std::make_shared<Uniform_entry<glm::vec3>>(m_diffuse));
        m_shader->add_uniform("material.specular", std::make_shared<Uniform_entry<glm::vec3>>(m_specular));
        m_shader->add_uniform("material.shininess", std::make_shared<Uniform_entry<float>>(m_shininess));

    }
};


class Instanced_phong_material : public Instanced_material , public Phong_material_base {
public:
    Instanced_phong_material(
        Pipeline_state pipeline_state,
        std::unordered_map<std::string, Binded_texture> textures,
        std::shared_ptr<Shader> shader,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float shininess,
        const std::vector<glm::mat4>& instance_tranforms
    ) : Instanced_material(
        Material_type::PHONG,
        pipeline_state,
        textures,
        shader,
        instance_tranforms
    ), Phong_material_base(
        ambient,
        diffuse,
        specular,
        shininess
    ) {}

    ~Instanced_phong_material() = default;

    virtual void upload_uniform(const std::shared_ptr<Node>& node, const std::shared_ptr<Camera_setting>& camera_setting, const std::shared_ptr<Light_setting>& light_setting) override {
        Instanced_material::upload_uniform(node, camera_setting, light_setting);
        m_shader->add_uniform("material.ambient", std::make_shared<Uniform_entry<glm::vec3>>(m_ambient));
        m_shader->add_uniform("material.diffuse", std::make_shared<Uniform_entry<glm::vec3>>(m_diffuse));
        m_shader->add_uniform("material.specular", std::make_shared<Uniform_entry<glm::vec3>>(m_specular));
        m_shader->add_uniform("material.shininess", std::make_shared<Uniform_entry<float>>(m_shininess));
    }

};

}