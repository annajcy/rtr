#include "engine/runtime/framework/component/custom/ping_pong_component.h"
#include "engine/runtime/framework/component/shadow_caster/shadow_caster_component.h"

#include "engine/editor/editor.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/function/render/object/skybox.h"
#include "engine/runtime/function/render/object/geometry.h"

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/custom/rotate_component.h"
#include "engine/runtime/framework/component/light/light_component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/framework/core/game_object.h"
#include "engine/runtime/framework/core/scene.h"
#include "engine/runtime/framework/core/world.h"

#include "engine/runtime/resource/file_service.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/runtime.h"

#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>

using namespace rtr;

int main() {

    Engine_runtime_descriptor engine_runtime_descriptor{};
    auto runtime = Engine_runtime::create(engine_runtime_descriptor);
    auto editor = editor::Editor::create(runtime);

    auto world = World::create("world1");
    runtime->world() = world;

    auto bk_image = Image::create(
        Image_format::RGB_ALPHA, 
        File_ser::get_instance()->get_absolute_path(
            "assets/image/skybox/spherical/bk.jpg"
        )
    );
    
    auto height_map = Image::create(
        Image_format::RGB_ALPHA,
        File_ser::get_instance()->get_absolute_path(
            "assets/image/bricks/disp.jpg"
        )
    );

    auto normal_map = Image::create(
        Image_format::RGB_ALPHA,
        File_ser::get_instance()->get_absolute_path(
            "assets/image/bricks/bricks_normal.jpg"
        )
    );

    auto main_tex = Image::create(
        Image_format::RGB_ALPHA, 
        File_ser::get_instance()->get_absolute_path(
            "assets/image/bricks/bricks.jpg"
        )
    );

    auto plane_main_tex = Image::create(
        Image_format::RGB_ALPHA, 
        File_ser::get_instance()->get_absolute_path(
            "assets/image/grass/grass.jpg"
        )
    );

    auto phong_material_settings = Phong_material_settings::create();
    auto parallax_settings = Parallax_settings::create();
    auto shadow_settings = Shadow_settings::create();

    auto go_texture_settings = Phong_texture_settings::create();
    go_texture_settings->albedo_map = Texture_2D::create_image(main_tex);
    go_texture_settings->normal_map = Texture_2D::create_image(normal_map);
    go_texture_settings->height_map = Texture_2D::create_image(height_map);

    auto plane_texture_settings = Phong_texture_settings::create();
    plane_texture_settings->albedo_map = Texture_2D::create_image(plane_main_tex);

    auto phong_shader = Phong_shader::create();
    phong_shader->generate_all_shader_variants();

    auto go_material = Phong_material::create(phong_shader);
    go_material->phong_material_settings = phong_material_settings;
    go_material->phong_texture_settings = go_texture_settings;
    go_material->parallax_settings = parallax_settings;
    go_material->shadow_settings = shadow_settings;

    auto plane_material = Phong_material::create(phong_shader);
    plane_material->phong_texture_settings = plane_texture_settings;
    plane_material->phong_material_settings = phong_material_settings;
    plane_material->parallax_settings = parallax_settings;
    plane_material->shadow_settings = shadow_settings;

    auto scene = world->add_scene(Scene::create("scene1"));
    world->set_current_scene(scene);

    // auto spherical = Skybox::create(Texture_image::create(bk_image));
    // scene->set_skybox(spherical);

    auto cubemap = Skybox::create(Texture_cubemap::create_image(std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>{
        {Texture_cubemap_face::BACK, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/back.jpg", false)},
        {Texture_cubemap_face::BOTTOM, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/bottom.jpg", false)},
        {Texture_cubemap_face::FRONT, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/front.jpg", false)},
        {Texture_cubemap_face::LEFT, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/left.jpg", false)},
        {Texture_cubemap_face::RIGHT, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/right.jpg", false)},
        {Texture_cubemap_face::TOP, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/top.jpg", false)}
    }));
    scene->set_skybox(cubemap);

    auto camera_game_object = scene->add_game_object(Game_object::create("camera"));
    auto camera_node = camera_game_object->add_component<Node_component>()->node();
    camera_node->set_position(glm::vec3(0, 0, 60));
    camera_node->look_at_point(glm::vec3(0, 0, 0));

    auto camera_component = camera_game_object->add_component<Perspective_camera_component>();
    camera_component->add_resize_callback(runtime->rhi_window());
    
    auto camera_control_component = camera_game_object->add_component<Trackball_camera_control_component>();

    auto sphere = scene->add_game_object(Game_object::create("go1"));
    auto sphere_node = sphere->add_component<Node_component>()->node();
    sphere_node->set_position(glm::vec3(-2, 0, 0));

    auto sphere_mesh_renderer = sphere->add_component<Mesh_renderer_component>()->mesh_renderer();
    sphere_mesh_renderer->geometry() = Geometry::create_sphere();
    sphere_mesh_renderer->material() = go_material;

    auto box = scene->add_game_object(Game_object::create("go2"));
    auto box_node = box->add_component<Node_component>()->node();
    box_node->set_position(glm::vec3(0, 0, 0));

    auto rotate_component = box->add_component<Rotate_component>();
    rotate_component->speed() = 0.1f;

    auto ping_pong_component = box->add_component<Ping_pong_component>();
    ping_pong_component->position() = glm::vec3(0, 1, 0);
    ping_pong_component->speed() = 0.002f;

    auto box_mesh_renderer = box->add_component<Mesh_renderer_component>()->mesh_renderer();
    box_mesh_renderer->geometry() = Geometry::create_box();
    box_mesh_renderer->material() = go_material;

    auto plane = scene->add_game_object(Game_object::create("plane"));
    auto plane_node = plane->add_component<Node_component>()->node();
    plane_node->set_position(glm::vec3(0, -1, 0));
    plane_node->look_at_direction(glm::vec3(0, 1, 0));
    plane_node->set_scale(glm::vec3(5.0));
    auto plane_mesh_renderer = plane->add_component<Mesh_renderer_component>()->mesh_renderer();
    plane_mesh_renderer->geometry() = Geometry::create_plane();
    plane_mesh_renderer->material() = plane_material;

    auto dl_game_object = scene->add_game_object(Game_object::create("dl"));
    auto dl_node = dl_game_object->add_component<Node_component>()->node();
    dl_node->look_at_direction(glm::vec3(0, -1, 0));
    dl_node->set_position(glm::vec3(0, 3, 0));
    auto dl = dl_game_object->add_component<Directional_light_component>();
    auto dl_shadow_caster = dl_game_object->add_component<Directional_light_shadow_caster_component>();
    dl_shadow_caster->shadow_caster()->shadow_map() = Texture_2D::create_depth_attachemnt(2048, 2048);

    auto pl0_game_object = scene->add_game_object(Game_object::create("pl0"));
    auto pl0_node = pl0_game_object->add_component<Node_component>()->node();
    pl0_node->set_position(glm::vec3(1, 0, 0));
    auto pl0 = pl0_game_object->add_component<Point_light_component>()->point_light();
    pl0->color() = glm::vec3(0, 1, 0);

    auto pl1_game_object = scene->add_game_object(Game_object::create("pl1"));
    auto pl1_node = pl1_game_object->add_component<Node_component>()->node();
    pl1_node->set_position(glm::vec3(-1, 0, 0));
    auto pl1 = pl1_game_object->add_component<Point_light_component>()->point_light();
    pl1->color() = glm::vec3(0, 0, 1);

    auto sl0_game_object = scene->add_game_object(Game_object::create("sl0"));
    auto sl0_node = sl0_game_object->add_component<Node_component>()->node();
    sl0_node->set_position(glm::vec3(0, 0, 1.0));
    sl0_node->look_at_direction(glm::vec3(0, 0, -1));
    auto sl0 = sl0_game_object->add_component<Spot_light_component>()->spot_light();
    sl0->inner_angle() = 15.0f;
    sl0->outer_angle() = 20.0f;
    sl0->color() = glm::vec3(1, 1, 0);
    sl0->intensity() = 0.5f;

    auto sl1_game_object = scene->add_game_object(Game_object::create("sl1"));
    auto sl1_node = sl1_game_object->add_component<Node_component>()->node();
    sl1_node->set_position(glm::vec3(0.0, 1.0, 0.0));
    sl1_node->look_at_direction(glm::vec3(0.0, -1.0, 0.0));
    auto sl1 = sl1_game_object->add_component<Spot_light_component>()->spot_light();
    sl1->color() = glm::vec3(1, 1, 0);
    sl1->intensity() = 0.5f;
    
    box_node->add_child(sphere_node, true);

    int cubes_per_side_x = 30; 
    int cubes_per_side_z = 30;  
    int layers_y = 7;          
    float spacing = 2.0f;      // Spacing between cube centers

    float offset_x = (cubes_per_side_x - 1) * spacing / 2.0f;
    float offset_z = (cubes_per_side_z - 1) * spacing / 2.0f;

    int cube_count = 0;
    for (int y_layer = 0; y_layer < layers_y; ++y_layer) {
        for (int i = 0; i < cubes_per_side_x; ++i) {
            for (int j = 0; j < cubes_per_side_z; ++j) {
                std::string cube_name = "auto_cube_" + std::to_string(cube_count++);
                auto cube_go = scene->add_game_object(Game_object::create(cube_name));
                auto cube_go_node = cube_go->add_component<Node_component>()->node();

                float pos_x = (i * spacing) - offset_x;
                float pos_y = y_layer * spacing; // Stagger layers upwards
                float pos_z = (j * spacing) - offset_z;
                
                cube_go_node->set_position(glm::vec3(pos_x, pos_y, pos_z));

                // Add MeshRendererComponent and share geometry and material
                auto new_cube_mesh_renderer_component = cube_go->add_component<Mesh_renderer_component>();
                auto new_cube_mesh_renderer = new_cube_mesh_renderer_component->mesh_renderer();

                auto rotate_component = cube_go->add_component<Rotate_component>();
                rotate_component->speed() = 0.1f;
                
                // Share the geometry and material from the original box_mesh_renderer
                new_cube_mesh_renderer->geometry() = Geometry::create_box();
                new_cube_mesh_renderer->material() = go_material;
            }
        }
    }

    editor->get_panel<editor::Parallax_settings_panel>("parallax settings")->set_parallax_settings(parallax_settings);
    editor->get_panel<editor::Phong_material_settings_panel>("phong material settings")->set_phong_material_settings(phong_material_settings);
    editor->get_panel<editor::Shadow_settings_panel>("shadow settings")->set_shadow_settings(shadow_settings);

    editor->run();

    return 0;
}