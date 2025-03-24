#include "engine/global/base.h"
#include "engine/runtime/geometry.h"
#include "engine/runtime/input.h"
#include "engine/runtime/loader/image_loader_stb_image.h"
#include "engine/runtime/loader/text_loader.h"
#include "engine/runtime/material/material.h"
#include "engine/runtime/material/phong_material.h"
#include "engine/runtime/renderer.h"
#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/device/rhi_device_opengl.h"
#include "engine/runtime/shader.h"
#include "engine/runtime/texture.h"
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>

using namespace rtr;

int main() {

    RHI_device_descriptor device_descriptor{};
    device_descriptor.width = 800;
    device_descriptor.height = 600;
    device_descriptor.title = "RTR Engine";

    auto image_loader = std::make_shared<Image_loader_stb>();
    auto device = std::make_shared<RHI_device_OpenGL>(device_descriptor);
    auto input = std::make_shared<Input>(device->window());

    auto renderer = std::make_shared<Renderer>(device);

    //scene
    auto scene = std::make_shared<Scene>();

    //cubemap mesh (spherical)

    auto skybox_geometry = Geometry::create_box(100.0f);
    auto skybox_material = std::make_shared<Material> (
        Pipeline_state::opaque_pipeline_state(),
        std::unordered_map<std::string, Binded_texture> {
            {"main_tex", Binded_texture{0,
                std::make_shared<Texture_2D>(image_loader->load_from_path(Image_format::RGB_ALPHA, "assets/image/bk.jpg"))
            }}
        },
        std::make_shared<Shader>(
            std::unordered_map<Shader_type, std::string> {
                {Shader_type::VERTEX, Text_loader::load_from_file("assets/shaders/vertex/skybox.vert")},
                {Shader_type::FRAGMENT, Text_loader::load_from_file("assets/shaders/fragment/skybox_spherical.frag")}
            }
        )
    );

    skybox_material->pipeline_state().depth_state.function = Depth_function::LESS_EQUAL;
    skybox_material->pipeline_state().depth_state.write_enable = false;
    skybox_material->pipeline_state().cull_state.enable = false;
    auto skybox_mesh = std::make_shared<Mesh>(skybox_geometry, skybox_material);

    scene->add_child(skybox_mesh);

    //lightings

    auto directional_light = std::make_shared<Directional_light>();
    directional_light->look_at_direction(glm::vec3(1.0, -1.0, -1.0));
	directional_light->intensity() = 0.5f;
    directional_light->is_main() = true;

    auto ambient_light = std::make_shared<Ambient_light>();
	ambient_light->intensity() = 0.15f;
	
	auto spot_light = std::make_shared<Spot_light>();
	spot_light->position() = glm::vec3(0.0f, 0.0f, 2.0f);
	spot_light->look_at_direction(glm::vec3(0.0, 0.0f, -1.0f));
	spot_light->inner_angle() = 5.0f;
	spot_light->outer_angle() = 10.0f;
	spot_light->color() = glm::vec3(1.0, 1.0, 0.0);

	auto point_light_1 = std::make_shared<Point_light>();
	point_light_1->position() = glm::vec3(1.0f, 0.0f, 0.0f);
	point_light_1->color() = glm::vec3(1.0f, 0.0f, 0.0f);

	auto point_light_2 = std::make_shared<Point_light>();
	point_light_2->position() = glm::vec3(0.0f, 1.0f, 0.0f);
	point_light_2->color() = glm::vec3(0.0f, 1.0f, 0.0f);

	auto point_light_3 = std::make_shared<Point_light>();
	point_light_3->position() = glm::vec3(0.0f, 0.0f, -1.0f);
	point_light_3->color() = glm::vec3(0.0f, 0.0f, 1.0f);

    scene->add_child(directional_light);
    scene->add_child(ambient_light);
    scene->add_child(spot_light);
    scene->add_child(point_light_1);
    scene->add_child(point_light_2);
    scene->add_child(point_light_3);

    //camera

    auto camera = std::make_shared<Perspective_camera>(
		60.0f,
		(float) device->window()->width() / (float) device->window()->height(),
		0.1f, 
		50.0f
	);

	camera->position() = glm::vec3(0.0f, 0.0f, 5.0f);
	camera->look_at_point(glm::vec3(0.0f, 0.0f, 0.0f));
	auto camera_control = std::make_shared<Trackball_camera_control>(input, camera);

    scene->add_child(camera);

    //box mesh
    auto box_geometry = Geometry::create_box(2.0f);
    auto box_material = std::make_shared<Phong_material> (
        Pipeline_state::opaque_pipeline_state(),
        std::unordered_map<std::string, Binded_texture> {
            {"main_tex", Binded_texture{0,
                std::make_shared<Texture_2D>(image_loader->load_from_path(Image_format::RGB_ALPHA, "assets/image/default_texture.jpg"))
            }}
        },
        std::make_shared<Shader>(
            std::unordered_map<Shader_type, std::string> {
                {Shader_type::VERTEX, Text_loader::load_from_file("assets/shaders/vertex/world_n.vert")},
                {Shader_type::FRAGMENT, Text_loader::load_from_file("assets/shaders/fragment/phong.frag")}
            }
        ),
        glm::vec3(0.2f),
        glm::vec3(0.8f),
        glm::vec3(0.5f),
        32.0f
    );

    auto box_mesh = std::make_shared<Mesh>(box_geometry, box_material);

    scene->add_child(box_mesh);
    

    while (renderer->device()->window()->is_active()) {
        device->window()->update();
        device->clear();

        renderer->scene() = scene;
        renderer->render();

        std::cout << input->mouse_x() << " " << input->mouse_y() << std::endl;
        std::cout << input->mouse_dx() << " " << input->mouse_dy() << std::endl;
        std::cout << input->mouse_scroll_dx() << " " << input->mouse_scroll_dy() << std::endl;

        device->window()->on_frame_end();
        
    }


    
    
    return 0;
}