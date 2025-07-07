#include "engine/editor/panel/parallax_setting_panel.h"
#include "engine/editor/panel/phong_material_setting_panel.h"
#include "engine/editor/panel/shadow_setting_panel.h"
#include "engine/editor/panel/fps_panel.h"

#include "engine/runtime/framework/component/custom/ping_pong_component.h"
#include "engine/runtime/framework/component/shadow_caster/shadow_caster_component.h"

#include "engine/editor/editor.h"
#include "engine/runtime/framework/plugin/model_loader.h"
#include "engine/runtime/function/render/material/material.h"
#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/function/render/material/shading/phong_material.h"
#include "engine/runtime/function/render/utils/skybox.h"
#include "engine/runtime/function/render/frontend/geometry.h"

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
#include "engine/runtime/resource/loader/image.h"
#include "engine/runtime/runtime.h"

#include "engine/runtime/function/render/pipeline/forward_pipeline.h"

#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>

using namespace rtr;

int main() {

    Engine_runtime_descriptor engine_runtime_descriptor{};
    auto runtime = Engine_runtime::create(engine_runtime_descriptor);
    auto forward_pipeline = Forward_pipeline::create(
        runtime->rhi_global_resource()
    );
    runtime->render_system()->set_render_pipeline(forward_pipeline);
    
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
    
    auto mary = Model_assimp::create(
        File_ser::get_instance()->get_absolute_path(
            //"assets/model/mary/Marry.obj"
            "assets/model/backpack/backpack.obj"
        )
    );

    auto parallax_settings = forward_pipeline->parallax_setting();
    auto shadow_settings = forward_pipeline->shadow_setting();

    auto phong_material_settings = Phong_material_setting::create();
    auto go_texture_settings = Phong_texture_setting::create();
    go_texture_settings->albedo_map = Texture_2D::create_image(main_tex);
    go_texture_settings->normal_map = Texture_2D::create_image(normal_map, Texture_internal_format::RGB_ALPHA_8F);
    go_texture_settings->height_map = Texture_2D::create_image(height_map, Texture_internal_format::RGB_ALPHA_8F);

    auto plane_texture_settings = Phong_texture_setting::create();
    plane_texture_settings->albedo_map = Texture_2D::create_image(plane_main_tex);

    auto phong_shader = Phong_material::phong_shader();
    phong_shader->generate_all_shader_variants();

    auto go_material = Phong_material::create();
    go_material->phong_material_settings = phong_material_settings;
    go_material->phong_texture_settings = go_texture_settings;
    go_material->parallax_settings = parallax_settings;
    go_material->shadow_settings = shadow_settings;

    auto plane_material = Phong_material::create();
    plane_material->phong_texture_settings = plane_texture_settings;
    plane_material->phong_material_settings = phong_material_settings;
    plane_material->parallax_settings = parallax_settings;
    plane_material->shadow_settings = shadow_settings;

    auto scene = world->add_scene(Scene::create("scene1"));
    world->set_current_scene(scene);

    //scene->set_skybox(Skybox::create(Texture_2D::create_image(bk_image)));
    scene->set_skybox(Skybox::create(
        Texture_cubemap::create_image(
        std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>{
            {Texture_cubemap_face::BACK, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/back.jpg", false)},
            {Texture_cubemap_face::BOTTOM, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/bottom.jpg", false)},
            {Texture_cubemap_face::FRONT, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/front.jpg", false)},
            {Texture_cubemap_face::LEFT, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/left.jpg", false)},
            {Texture_cubemap_face::RIGHT, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/right.jpg", false)},
            {Texture_cubemap_face::TOP, Image::create(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/top.jpg", false)}
    })));

    auto bag_root_go = scene->add_model("mary", mary, Model_loader<Phong_material>::create(
        forward_pipeline->shadow_setting(),
        forward_pipeline->parallax_setting()
    ));

    auto bag_root_go_node = bag_root_go->get_component<Node_component>()->node();
    bag_root_go_node->set_position(glm::vec3(0, 2, 0));
    //bag_root_go_node->set_scale(glm::vec3(7.0f));
    auto bag_rot = bag_root_go->add_component<Rotate_component>();
    bag_rot->speed() = 0.01f;

    auto camera_game_object = scene->add_game_object(Game_object::create("camera"));
    auto camera_node = camera_game_object->add_component<Node_component>()->node();
    camera_node->set_position(glm::vec3(0, 1, 5));
    camera_node->look_at_point(glm::vec3(0, 0, 0));
    camera_node->rotate(180, camera_node->front());

    auto camera_component = camera_game_object->add_component<Perspective_camera_component>();
    camera_component->add_resize_callback(runtime->rhi_global_resource().window);
    
    auto camera_control_component = camera_game_object->add_component<Trackball_camera_control_component>();

    auto sphere = scene->add_game_object(Game_object::create("go1"));
    auto sphere_node = sphere->add_component<Node_component>()->node();
    sphere_node->set_position(glm::vec3(-3, 0, 0));

    auto sphere_mesh_renderer = sphere->add_component<Mesh_renderer_component>()->mesh_renderer();
    sphere_mesh_renderer->geometry() = Geometry::create_sphere();
    sphere_mesh_renderer->material() = go_material;

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
    dl_node->look_at_direction(glm::vec3(1, -1, 1));
    dl_node->set_position(glm::vec3(0, 3, 0));
    auto dl = dl_game_object->add_component<Directional_light_component>();
    auto dl_shadow_caster = dl_game_object->add_component<Directional_light_shadow_caster_component>();
    dl_shadow_caster->shadow_caster()->shadow_map() = Texture_2D::create_color_attachemnt_rg(
        2048, 2048, 
        6
    );
    dl_shadow_caster->camera_orthographic_size() = 17.0f;

    auto sl1_game_object = scene->add_game_object(Game_object::create("sl1"));
    auto sl1_node = sl1_game_object->add_component<Node_component>()->node();
    sl1_node->set_position(glm::vec3(0.0, 1.0, 0.0));
    sl1_node->look_at_direction(glm::vec3(0.0, -1.0, 0.0));
    auto sl1 = sl1_game_object->add_component<Spot_light_component>()->spot_light();
    sl1->color() = glm::vec3(1, 1, 0);
    sl1->intensity() = 0.5f;

    auto editor = editor::Editor::create(
        runtime, 
        {
            editor::Shadow_settings_panel::create("shadow settings"),
            editor::Phong_material_settings_panel::create("phong material settings"),
            editor::Parallax_settings_panel::create("parallax settings"),
            editor::FPS_panel::create("fps")
    });

    editor->get_panel<editor::Shadow_settings_panel>("shadow settings")->set_shadow_settings(shadow_settings);
    editor->get_panel<editor::Parallax_settings_panel>("parallax settings")->set_parallax_settings(parallax_settings);
    editor->get_panel<editor::Phong_material_settings_panel>("phong material settings")->set_phong_material_settings(phong_material_settings);
    
    editor->run();

    return 0;
}