
#include "engine/runtime/framework/component/shadow_caster/shadow_caster_component.h"
#include "engine/editor/editor.h"
#include "engine/runtime/framework/plugin/model_loader.h"
#include "engine/runtime/function/render/render_material/material.h"
#include "engine/runtime/function/render/render_pipeline/render_pipeline.h"
#include "engine/runtime/function/render/render_material/material.h"
#include "engine/runtime/function/render/render_frontend/shader.h"
#include "engine/runtime/function/render/render_frontend/texture.h"
#include "engine/runtime/function/render/render_utils/skybox.h"
#include "engine/runtime/function/render/render_frontend/geometry.h"

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"

#include "engine/runtime/framework/component/light/light_component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/framework/component/node/node_component.h"

#include "engine/runtime/framework/component/custom/rotate_component.h"
#include "engine/runtime/framework/component/custom/ping_pong_component.h"

#include "engine/runtime/framework/core/game_object.h"
#include "engine/runtime/framework/core/scene.h"
#include "engine/runtime/framework/core/world.h"

#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_texture.h"
#include "engine/runtime/resource/file_service.h"
#include "engine/runtime/resource/loader/image.h"
#include "engine/runtime/resource/loader/model.h"
#include "engine/runtime/runtime.h"

#include "engine/runtime/function/render/render_pipeline/forward_render_pipeline.h"

#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

using namespace rtr;

int main() {

    Engine_runtime_descriptor engine_runtime_descriptor{};
    auto runtime = Engine_runtime::create(engine_runtime_descriptor);
    runtime->render_system()->set_render_pipeline(Forward_render_pipeline::create(
        runtime->rhi_global_resource()
    ));
    
    auto world = World::create("world1");
    runtime->world() = world;

    auto sponza = Model_assimp::create(
        File_ser::get_instance()->get_absolute_path(
            "assets/model/sponza/sponza.obj"
            //"assets/model/mary/Marry.obj"
        )
    );
    
    auto scene = world->add_scene(Scene::create("scene1"));
    world->set_current_scene(scene);

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

    auto sponza_root_go = scene->add_model<Phong_material>("sponza", sponza);

    sponza_root_go->get_component<Node_component>()->node()->set_scale(glm::vec3(0.0005f));
    // auto sponza_rot = sponza_root_go->add_component<Rotate_component>();
    // sponza_rot->speed() = 0.01f;

    auto camera_game_object = scene->add_game_object(Game_object::create("camera"));
    auto camera_node = camera_game_object->add_component<Node_component>()->node();
    camera_node->set_position(glm::vec3(0, 0, 5));
    camera_node->look_at_point(glm::vec3(0, 0, 0));
    //camera_node->rotate(180, camera_node->front());

    auto camera_component = camera_game_object->add_component<Perspective_camera_component>();
    camera_component->add_resize_callback(runtime->rhi_global_resource().window);
    
    auto camera_control_component = camera_game_object->add_component<Trackball_camera_control_component>();

    auto dl_game_object = scene->add_game_object(Game_object::create("dl"));
    auto dl_node = dl_game_object->add_component<Node_component>()->node();
    dl_node->look_at_direction(glm::vec3(0, -1, 0));
    dl_node->set_position(glm::vec3(0, 3, 0));
    auto dl = dl_game_object->add_component<Directional_light_component>();
    auto dl_shadow_caster = dl_game_object->add_component<Directional_light_shadow_caster_component>();
    dl_shadow_caster->shadow_caster()->shadow_map() = Texture_2D::create_depth_attachemnt(2048, 2048);
    dl_shadow_caster->camera_orthographic_size() = 17.0f;

    auto editor = editor::Editor::create(
        runtime, 
        {editor::FPS_panel::create("fps")
    });

    editor->run();

    return 0;
}