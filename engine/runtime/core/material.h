#pragma once

#include "engine/runtime/core/shader.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/resource/loader/image_loader.h"

namespace rtr {

class Material {
    
protected:
    Material_type m_material_type{};
    std::shared_ptr<Shader> m_shader{}; 

public:
    Material(
        Material_type material_type, 
        const std::shared_ptr<Shader>& shader
    ) : m_material_type(material_type),
        m_shader(shader) {}
    virtual ~Material() = default;

    Material_type type() const { return m_material_type; }
    const std::shared_ptr<Shader>& shader() const { return m_shader; }

    virtual Shader::feature_set get_shader_feature_set() const = 0;
    const std::shared_ptr<Shader_program> get_shader_program() const {
        auto shader_variant = m_shader->get_shader_variant(get_shader_feature_set());
        return shader_variant;
    }

    virtual bool is_transparent() const = 0;

    Pipeline_state get_pipeline_state() const {
        if (is_transparent()) {
            return Pipeline_state::translucent_pipeline_state();
        } else {
            return Pipeline_state::opaque_pipeline_state();
        }
    }
};

class Phong_material : public Material {
public:
    bool is_receive_shadows{false};
    std::shared_ptr<Image> albedo_map{};
    std::shared_ptr<Image> alpha_map{};
    std::shared_ptr<Image> normal_map{};
    std::shared_ptr<Image> height_map{};
    std::shared_ptr<Image> specular_map{};

    float transparency{1.0f};
    glm::vec3 ambient = glm::vec3(0.2f);     // 环境反射系数
    glm::vec3 diffuse = glm::vec3(1.0f);     // 漫反射系数 (或使用 albedo_map)
    glm::vec3 specular = glm::vec3(0.5f);    // 镜面反射系数
    float shininess = 32.0f;          

public:
    Phong_material() : Material(
        Material_type::PHONG, 
        Shader::create_phong_shader()
    ) {}
    ~Phong_material() = default;

    bool is_transparent() const override { return transparency < 1.0f || has_alpha_map(); }
    bool has_albedo_map() const { return albedo_map != nullptr; }
    bool has_alpha_map() const { return alpha_map != nullptr; }
    bool has_height_map() const { return height_map != nullptr; }
    bool has_specular_map() const { return specular_map != nullptr; }
    bool has_normal_map() const { return normal_map != nullptr; }

    static std::shared_ptr<Phong_material> create() {
        return std::make_shared<Phong_material>();
    }

    Shader::feature_set get_shader_feature_set() const override {
        Shader::feature_set feature_set{};
        if (is_receive_shadows) {
            feature_set.set(static_cast<size_t>(Shader_feature::SHADOWS));
        }
        if (has_normal_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::NORMAL_MAP));
        }
        if (has_albedo_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::ALBEDO_MAP));
        }
        if (has_height_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::HEIGHT_MAP));
        }
        if (has_alpha_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::ALPHA_MAP));
        }
        if (has_specular_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::SPECULAR_MAP));
        }
        return feature_set;
    }
};

class PBR_material : public Material {
public:
    bool is_receive_shadows{false};
    std::shared_ptr<Image> albedo_map{};
    std::shared_ptr<Image> alpha_map{};
    std::shared_ptr<Image> normal_map{};
    std::shared_ptr<Image> height_map{};
    
    std::shared_ptr<Image> metallic_map{};
    std::shared_ptr<Image> roughness_map{};
    std::shared_ptr<Image> ao_map{};
    std::shared_ptr<Image> emissive_map{}; // 新增自发光贴图

    glm::vec3 base_color{};
    float metallic_factor = {1.0f};      // 金属度系数
    float roughness_factor = {0.5f};       // 粗糙度系数
    float transparency {1.0f};

public:

    PBR_material() : Material(Material_type::PBR, Shader::create_pbr_shader()) {}
    ~PBR_material() = default;

    bool is_transparent() const override { return transparency < 1.0f || has_alpha_map(); }
    bool has_height_map() const { return height_map!= nullptr; }
    bool has_normal_map() const { return normal_map != nullptr; }
    bool has_emissive_map() const { return emissive_map != nullptr; }
    bool has_metallic_map() const { return metallic_map!= nullptr; }
    bool has_roughness_map() const { return roughness_map!= nullptr; }
    bool has_ao_map() const { return ao_map != nullptr; }
    bool has_albedo_map() const { return albedo_map != nullptr; }
    bool has_alpha_map() const { return alpha_map != nullptr; }

    static std::shared_ptr<PBR_material> create() {
        return std::make_shared<PBR_material>();
    }
    
    Shader::feature_set get_shader_feature_set() const override {
        Shader::feature_set feature_set{};
        if (is_receive_shadows) {
            feature_set.set(static_cast<size_t>(Shader_feature::SHADOWS));
        }
        if (has_albedo_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::ALBEDO_MAP));
        }
        if (has_alpha_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::ALPHA_MAP));
        }
        if (has_height_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::HEIGHT_MAP));
        }
        if (has_metallic_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::METALLIC_MAP));
        }
        if (has_roughness_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::ROUGHNESS_MAP));
        }
        if (has_ao_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::AO_MAP));
        }
        if (has_normal_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::NORMAL_MAP));
        }
        if (has_emissive_map()) {
            feature_set.set(static_cast<size_t>(Shader_feature::EMISSIVE_MAP));
        }

        return feature_set;
    }
};

class Skybox_cubemap_material : public Material {
public:
    std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>> cubemap_faces{};
    Skybox_cubemap_material() : Material(
        Material_type::SKYBOX_CUBEMAP, 
        Shader::create_skybox_cubemap_shader()
    ) {}
    ~Skybox_cubemap_material() = default;
    bool is_transparent() const override { return false; }
    Shader::feature_set get_shader_feature_set() const override {
        Shader::feature_set feature_set{};
        return feature_set;
    }
};

class SkyBox_spherical_material : public Material {
public:
    std::shared_ptr<Image> skybox_image{};
    SkyBox_spherical_material() : Material(
        Material_type::SKYBOX_SPHERICAL, 
        Shader::create_skybox_cubemap_shader()
    ) {}
    ~SkyBox_spherical_material() = default;
    bool is_transparent() const override { return false; }
    Shader::feature_set get_shader_feature_set() const override {
        Shader::feature_set feature_set{};
        return feature_set;
    }

};

}