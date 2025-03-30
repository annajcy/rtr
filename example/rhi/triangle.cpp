#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_device.h"
#include "engine/runtime/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include <iterator>
#include <memory>
#include <unordered_map>

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

std::vector<unsigned int> indices = {
    0, 1, 2
};    

int main() {

    std::cout << "Hello, RHI!" << std::endl;

    auto device = std::make_shared<RHI_device_OpenGL>();
    auto window = device->create_window(800, 600, "RTR");

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
        std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
            {Shader_type::VERTEX, vertex_shader_code},
            {Shader_type::FRAGMENT, fragment_shader_code}
        },  
        std::unordered_map<std::string, RHI_uniform_entry> {}, 
        std::unordered_map<std::string, RHI_uniform_array_entry>{}
    );


    geometry->bind();
    shader_program->bind();

    while (window->is_active()) {
        window->on_frame_begin();

        device->check_error();
        geometry->draw();

        window->on_frame_end();
    }
    
    return 0;
}

