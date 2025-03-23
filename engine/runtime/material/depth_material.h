#pragma once
#include "material.h"
#include <memory>


namespace rtr {

class Depth_material_base {};

class Depth_material : public Material, public Depth_material_base {
public:
    Depth_material(
        const Pipeline_state& pipeline_state,
        const std::unordered_map<std::string, Binded_texture>& textures,
        const std::shared_ptr<Shader>& shader
    ) : Material(Material_type::DEPTH, pipeline_state, textures, shader), 
    Depth_material_base() {}

    void upload_uniform(std::shared_ptr<Shader>& shader) override {
        Material::upload_uniform(shader);
    }

};


class Instanced_depth_material : public Instanced_material, public Depth_material_base {
    Instanced_depth_material(
        const Pipeline_state& pipeline_state,
        const std::unordered_map<std::string, Binded_texture>& textures,
        const std::shared_ptr<Shader>& shader,
        const std::vector<glm::mat4> &instance_tranforms
    ) : Instanced_material(Material_type::DEPTH, pipeline_state, textures, shader, instance_tranforms),
        Depth_material_base() {}

    void upload_uniform(std::shared_ptr<Shader>& shader) override {
        Instanced_material::upload_uniform(shader);
    }

};

}