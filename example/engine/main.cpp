#include "engine/editor/editor.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/framework/game_object.h"
#include "engine/runtime/framework/scene.h"
#include "engine/runtime/framework/world.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/runtime.h"
#include <memory>

using namespace rtr;

int main() {

    auto bk_image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
         "assets/image/bk.jpg"
    );

    auto image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
         "assets/image/earth.png"
    );

    auto world = World::create("world1");
    auto scene = world->add_scene(Scene::create("scene1"));
    world->set_current_scene(scene);

    scene->set_skybox(std::make_shared<Texture2D>(bk_image));

    auto game_object = scene->add_game_object(Game_object::create("go1"));
 
    auto node = game_object->add_component<Node_component>(Node_component::create());

    auto camera = game_object->add_component<Perspective_camera_component>(Perspective_camera_component::create(
        45.0f,
        1.0f,
        0.1f,
        100.0f
    ));

    camera->node()->set_position(glm::vec3(0, 0, 3));
    camera->node()->look_at_point(glm::vec3(0, 0, 0));

    auto camera_control = game_object->add_component<Trackball_camera_control_component>(Trackball_camera_control_component::create());

    auto directional_light = game_object->add_component<Directional_light_component>(Directional_light_component::create());
    directional_light->intensity() = 1.0f;
    directional_light->node()->look_at_direction(glm::vec3(0, -1, 0));

    auto point_light = game_object->add_component<Point_light_component>(Point_light_component::create());
    point_light->intensity() = 5;
    point_light->node()->set_position(glm::vec3(-2, 0, 0));

    auto spot_light = game_object->add_component<Spot_light_component>(Spot_light_component::create());
    spot_light->intensity() = 5;
    spot_light->node()->set_position(glm::vec3(2, 0, 0));
    spot_light->inner_angle() = 5.0f;
    spot_light->outer_angle() = 10.0f;

    auto geometry = Geometry::create_box();
    auto material = Phong_material::create();

    material->is_receive_shadows = false;
    material->albedo_map = std::make_shared<Texture2D>(image);
    material->shininess = 32;
    material->transparency = 0.5f;

    auto mesh_renderer = game_object->add_component<Mesh_renderer_component>(Mesh_renderer_component::create(
        geometry,
        material
    ));

    Engine_runtime_descriptor engine_runtime_descriptor{};
    engine_runtime_descriptor.world = world;

    auto runtime = Engine_runtime::create(engine_runtime_descriptor);
    
    auto editor = editor::Editor::create(runtime);
    editor->run();

    return 0;
}