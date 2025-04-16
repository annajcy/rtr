#include "engine/runtime/global/base.h" 
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include <memory>

using namespace std;
using namespace rtr;

// Vertex Shader source code
const char* vertex_shader_source = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";
    
// Fragment Shader source code
const char* fragment_shader_source = R"(
#version 460 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

std::vector<float> vertices = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
};

std::vector<float> tex_coords = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f
};

std::vector<unsigned int> indices = {
    0, 1, 2
};    

int main() {

    auto device = std::make_shared<RHI_device_OpenGL>();
    auto window = device->create_window(800, 600, "RTR");
    auto screen_frame_buffer = device->create_screen_frame_buffer(window);

    auto position = device->create_vertex_buffer(
        Buffer_usage::STATIC, 
        Buffer_data_type::FLOAT, 
        Buffer_iterate_type::PER_VERTEX,
        3, 
        vertices.size() * sizeof(float), 
        vertices.data()
    );

    auto element = device->create_element_buffer(
        Buffer_usage::STATIC,
        indices.size(),
        indices.size() * sizeof(unsigned int),
        indices.data()
    );

    auto geometry = device->create_geometry(
        std::unordered_map<unsigned int, RHI_buffer::Ptr>{
            {0, position}
        }, 
        element
    );

    auto vertex_shader_code = device->create_shader_code(Shader_type::VERTEX, vertex_shader_source);
    auto fragment_shader_code = device->create_shader_code(Shader_type::FRAGMENT, fragment_shader_source);

    auto shader_program = device->create_shader_program(
        std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>{
            {Shader_type::VERTEX, vertex_shader_code},
            {Shader_type::FRAGMENT, fragment_shader_code}
        },
        {}
    );

    Clear_state clear_state = Clear_state::enabled();
    clear_state.color_clear_value = glm::vec4(0.1, 0.5, 0.3, 1.0);
    auto renderer = device->create_renderer(clear_state);


    while (window->is_active()) {
        window->on_frame_begin();

        device->check_error();
        renderer->init();
        renderer->clear(screen_frame_buffer);
        renderer->draw(
            shader_program,
            geometry,
            screen_frame_buffer
        );

        window->on_frame_end();
    }
    
    return 0;
}


