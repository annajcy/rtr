#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_device.h"
#include "engine/runtime/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include <iterator>
#include <unordered_map>

using namespace std;
using namespace rtr;

// Vertex Shader source code
const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";
    
// Fragment Shader source code
const char* fragmentShaderSource = R"(
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

    // RHI_device* device = new RHI_device_OpenGL();

    // auto window = device->create_window(800, 600, "RTR Engine");

    // auto ebo = device->create_buffer(Buffer_type::ELEMENT, Buffer_usage::STATIC, indices.size() * sizeof(unsigned int), indices.data());
    // auto vbo = device->create_buffer(Buffer_type::VERTEX, Buffer_usage::STATIC, vertices.size() * sizeof(float), vertices.data());

    // std::cout << static_cast<unsigned int>(reinterpret_cast<uintptr_t>(ebo->native_handle())) << std::endl;

    // auto geometry = device->create_geometry(std::unordered_map<unsigned int, Vertex_buffer_descriptor>{
    //    {0, Vertex_buffer_descriptor{
    //        vbo->guid(),
    //        Buffer_data_type::FLOAT,
    //        Buffer_iterate_type::PER_VERTEX,
    //        3
    //    }} 
    // }, Element_buffer_descriptor{
    //     ebo->guid(),
    //     static_cast<unsigned int>(ebo->data_size() / sizeof(unsigned int))
    // });

    // auto vertex_shader_code = device->create_shader_code(Shader_type::VERTEX, vertexShaderSource);
    // auto fragment_shader_code = device->create_shader_code(Shader_type::FRAGMENT, fragmentShaderSource);

    // auto shader_program = device->create_shader_program(std::unordered_map<Shader_type, unsigned int>{
    //     {Shader_type::VERTEX, vertex_shader_code->guid()},
    //     {Shader_type::FRAGMENT, fragment_shader_code->guid()}
    // },
    // std::unordered_map<std::string, RHI_uniform_entry>{},
    // std::unordered_map<std::string, RHI_uniform_array_entry>{});

    // shader_program->bind();
    // geometry->bind();

    // while (window->is_active()) {
    //     window->on_frame_begin();
    //     geometry->draw();
    //     device->check_error();
    //     window->on_frame_end();
    // }

    // delete geometry;
    // delete shader_program;
    // delete window;
    
    return 0;
}

