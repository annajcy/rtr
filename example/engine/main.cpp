#include "engine/runtime/core/material.h"
#include "engine/runtime/function/context/global_context.h"
#include "engine/runtime/function/framework/component/camera/camera_component.h"
#include "engine/runtime/function/framework/component/camera/camera_control_component.h"
#include "engine/runtime/function/framework/component/node/node_component.h"
#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/function/framework/game_object.h"
#include "engine/runtime/function/framework/scene.h"
#include "engine/runtime/function/framework/world.h"
#include "engine/runtime/function/framework/component/component_base.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/runtime.h"
#include <memory>

using namespace rtr;

int main() {

    auto game_object = Game_object::create("go1");

    auto node = Node_component::create();
    game_object->add_component<Node_component>(node);

    auto camera = Perspective_camera_component::create(
        45.0f,
        1.0f,
        0.1f,
        100.0f
    );

    game_object->add_component<Perspective_camera_component>(camera);

    auto camera_control = Trackball_camera_control_component::create();
    game_object->add_component<Trackball_camera_control_component>(camera_control);

    auto directional_light = Directional_light_component::create();
    game_object->add_component<Directional_light_component>(directional_light);

    auto geometry = Geometry::create_box();
    auto material = PBR_material::create();

    material->is_receive_shadows = true;
    material->albedo_map = Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/bk.jpg");
    material->roughness_map = Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/box.jpg");
    material->metallic_map = Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/box.jpg");

    auto mesh_renderer = Mesh_renderer_component::create(
        geometry,
        material
    );

    game_object->add_component<Mesh_renderer_component>(mesh_renderer);

    auto scene = Scene::create("scene1");
    scene->add_game_object(game_object);
    auto world = World::create("world1");
    world->add_scene(scene);
    world->set_current_scene(scene);

    Engine_runtime_descriptor engine_runtime_descriptor{};
    engine_runtime_descriptor.width = 800;
    engine_runtime_descriptor.height = 600;
    engine_runtime_descriptor.title = "RTR";
    engine_runtime_descriptor.world = world;

    auto runtime = Engine_runtime::create(engine_runtime_descriptor);

    runtime->run();

    return 0;
}