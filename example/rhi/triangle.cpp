#include "engine/global/base.h"

#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/device/rhi_device_opengl.h"

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
    
    RHI_device_descriptor descriptor{};

    descriptor.width = 800;
    descriptor.height = 600;
    descriptor.title = "RTR Engine";

    auto device = std::make_shared<RHI_device_OpenGL>(descriptor);

    device->pipeline_state()->set_profile(Pipeline_state_profile::OPAQUE);

    //prepare geometry

    auto vertex_attribute = device->create_vertex_buffer(
        Buffer_usage::STATIC, 
        Buffer_attribute_type::FLOAT, 
        3, 
        vertices.size(), 
        vertices.data()
    );

    auto element_buffer = device->create_element_buffer(
        Buffer_usage::STATIC,
        indices.size(),
        indices.data()
    );
    
    std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>> vertex_buffers = {
        {0, vertex_attribute}
    };

    auto geometry = device->create_geometry(vertex_buffers, element_buffer);

    //prepare shaders

    auto vertex_shader = device->create_shader_code(
        Shader_type::VERTEX,
        vertexShaderSource
    );

    auto fragment_shader = device->create_shader_code(
        Shader_type::FRAGMENT,
        fragmentShaderSource
    );

    std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> shaders = {
        {Shader_type::VERTEX, vertex_shader},
        {Shader_type::FRAGMENT, fragment_shader}
    };

    auto shader_program = device->create_shader_program(shaders);

    //prepare binding state

    device->binding_state()->set_geometry(geometry);
    device->binding_state()->set_shader_program(shader_program);

    while (device->window()->is_active()) {
        device->clear();
        device->draw();
        device->check_error();
        device->window()->update();
    }
    
    return 0;
}

