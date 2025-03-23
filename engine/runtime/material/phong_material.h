#pragma once
#include "material.h"

namespace rtr {

class Phong_material_base : public ISet_shader_uniform {
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
    void upload_uniform(std::shared_ptr<Shader>& shader) {
        shader->add_uniform("material.ambient", std::make_shared<Uniform_entry<glm::vec3>>(m_ambient));
        shader->add_uniform("material.diffuse", std::make_shared<Uniform_entry<glm::vec3>>(m_diffuse));
        shader->add_uniform("material.specular", std::make_shared<Uniform_entry<glm::vec3>>(m_specular));
        shader->add_uniform("material.shininess", std::make_shared<Uniform_entry<float>>(m_shininess));
    }
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

    void upload_uniform(std::shared_ptr<Shader>& shader) override {
        Material::upload_uniform(shader);
        Phong_material_base::upload_uniform(shader);
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

    void upload_uniform(std::shared_ptr<Shader>& shader) override {
        Instanced_material::upload_uniform(shader);
        Phong_material_base::upload_uniform(shader);
    }

};

}