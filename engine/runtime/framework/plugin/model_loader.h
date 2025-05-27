#pragma once

#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/framework/core/game_object.h"
#include "engine/runtime/function/render/object/attribute_buffer.h"
#include "engine/runtime/function/render/object/geometry.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/resource/loader/model_loader.h"
#include <memory>
#include <string>
#include <vector>

namespace rtr {


class Model_loader {
public:
    static std::vector<std::shared_ptr<Material>> load_materials(
        const std::shared_ptr<Model> &model
    ) {
        std::vector<std::shared_ptr<Material>> materials{};
        for (const auto& material : model->materials()) {
            materials.push_back(convert_material(material));
        }
        return materials;
    }

    static std::shared_ptr<Game_object> load_model(
        const std::string model_name,
        const std::shared_ptr<Model> &model,
        std::vector<std::shared_ptr<Game_object>>& game_objects
    ) {

        auto materials = load_materials(model);

        auto go = dfs(model_name, 
            model->root_node(), 
            materials, 
            game_objects
        );

        return go;
    }

private:

    static std::shared_ptr<Geometry> convert_geomerty(
        const std::shared_ptr<Model_geometry>& model_geometry
    ) {
        auto vertex_atrribute = std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> {};
        auto positions = Position_attribute::create(model_geometry->positions);
        vertex_atrribute.insert({0, positions});

        if (model_geometry->has_uvs()) {
            auto uvs = UV_attribute::create(model_geometry->uvs);
            vertex_atrribute.insert({1, uvs});
        }

        if (model_geometry->has_normals()) {
            auto normals = Normal_attribute::create(model_geometry->normals);
            vertex_atrribute.insert({2, normals});
        }

        if (model_geometry->has_tangents()) {
            auto tangents = Tangent_attribute::create(model_geometry->tangents);
            vertex_atrribute.insert({3, tangents});
        }

        auto element_attribute = Element_atrribute::create(model_geometry->indices);
        return Geometry::create(vertex_atrribute, element_attribute);
    }

    static std::shared_ptr<Material> convert_material(
        const std::shared_ptr<Model_material>& model_material,
        Material_type material_type = Material_type::PHONG
    ) {
        if (material_type == Material_type::PHONG) {
            auto material = Phong_material::create();

            auto phong_material_settings = Phong_material_settings::create();
            phong_material_settings->ka = model_material->ka;
            phong_material_settings->kd = model_material->kd;
            phong_material_settings->ks = model_material->ks;
            phong_material_settings->shininess = model_material->shininess;
            
            auto phong_texture_settings = Phong_texture_settings::create();
            phong_texture_settings->albedo_map = Texture_2D::create_image(model_material->map_albedo);
            phong_texture_settings->normal_map = Texture_2D::create_image(model_material->map_normal);
            phong_texture_settings->height_map = Texture_2D::create_image(model_material->map_height);
            phong_texture_settings->specular_map = Texture_2D::create_image(model_material->map_specular);
            phong_texture_settings->alpha_map = Texture_2D::create_image(model_material->map_alpha);

            auto parallax_settings = Parallax_settings::create();

            material->phong_texture_settings = phong_texture_settings;
            material->phong_material_settings = phong_material_settings;
            material->parallax_settings = parallax_settings;
            material->shadow_settings = Shadow_settings::create();

            return material;
        }
        
        return nullptr;
    }

    static std::shared_ptr<Game_object> dfs(
        const std::string& model_name,
        const std::shared_ptr<Model_node>& model_node, 
        const std::vector<std::shared_ptr<Material>> &materials,
        std::vector<std::shared_ptr<Game_object>>& game_objects
    ) {

        auto game_object = Game_object::create(model_name + std::to_string(game_objects.size()));
        auto node = game_object->add_component<Node_component>()->node();
        node->set_local_model_matrix(model_node->local_model_matrix);
        game_objects.push_back(game_object);

        for (const auto& mesh : model_node->meshes) {
            auto mesh_game_object = Game_object::create(model_name + std::to_string(game_objects.size()));
            auto mesh_node = mesh_game_object->add_component<Node_component>()->node();
            node->add_child(mesh_node);
            auto mesh_renderer = mesh_game_object->add_component<Mesh_renderer_component>()->mesh_renderer();
            // init mesh renderer
            mesh_renderer->material() = materials[mesh->material_index];
            mesh_renderer->geometry() = convert_geomerty(mesh->geometry);

            game_objects.push_back(mesh_game_object);
        }

        for (const auto& child : model_node->children) {
            node->add_child(
                dfs(model_name, 
                    child, 
                    materials, 
                    game_objects
                )->get_component<Node_component>()->node());
        }

        return game_object;
    }

};




};