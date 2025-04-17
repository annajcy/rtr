#include "engine/runtime/global/base.h" 
#include "engine/runtime/core/geometry.h"
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
// 修改片段着色器支持多目标输出
const char* fragment_shader_source = R"(
#version 460 core
layout(location = 0) out vec4 FragColor0;  // 第一个颜色附件
layout(location = 1) out vec4 FragColor1;  // 第二个颜色附件

void main()
{
    FragColor0 = vec4(1.0, 0.5, 0.2, 1.0); // 红色通道为主
    FragColor1 = vec4(0.2, 0.5, 1.0, 1.0); // 蓝色通道为主
}
)";

const char* vertex_shader_source1 = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aUV;
}
)";

// 修改后处理片段着色器
const char* fragment_shader_source1 = R"(
#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture0;  // 第一个颜色缓冲
uniform sampler2D texture1;  // 第二个颜色缓冲

void main()
{
    vec4 color0 = texture(texture0, TexCoords);
    vec4 color1 = texture(texture1, TexCoords);
    FragColor = mix(color0, color1, 0.5); // 混合两个缓冲区
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
        std::unordered_map<unsigned int, std::shared_ptr<RHI_buffer>>{
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

    // 创建两个颜色附件
    auto color_attachment0 = device->create_texture_2D(
        window->width(),
        window->height(),
        1,
        Texture_internal_format::RGB_ALPHA,
        {
            {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
            {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
        },
        {
            {Texture_filter_target::MIN, Texture_filter::LINEAR},
            {Texture_filter_target::MAG, Texture_filter::LINEAR}
        },
        Image_data{}
    );

    auto color_attachment1 = device->create_texture_2D(
        window->width(),
        window->height(),
        1,
        Texture_internal_format::RGB_ALPHA,
        {
            {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
            {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
        },
        {
            {Texture_filter_target::MIN, Texture_filter::LINEAR},
            {Texture_filter_target::MAG, Texture_filter::LINEAR}
        },
        Image_data{}
    );

    auto depth_attachment = device->create_texture_2D(
        window->width(),
        window->height(),
        1,
        Texture_internal_format::DEPTH_STENCIL_24_8,
        std::unordered_map<Texture_wrap_target, Texture_wrap>{
            {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
            {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
        },
        std::unordered_map<Texture_filter_target, Texture_filter>{
            // 修改过滤模式为NEAREST
            {Texture_filter_target::MIN, Texture_filter::NEAREST},
            {Texture_filter_target::MAG, Texture_filter::NEAREST}
        },
        Image_data{}
    );

    auto frame_buffer = device->create_frame_buffer(
        window->width(),
        window->height(),
        std::vector<std::shared_ptr<RHI_texture>>{
            color_attachment0, 
            color_attachment1
        },
        depth_attachment
    );

    auto vertex_shader_code1 = device->create_shader_code(Shader_type::VERTEX, vertex_shader_source1);
    auto fragment_shader_code1 = device->create_shader_code(Shader_type::FRAGMENT, fragment_shader_source1);
    
    auto shader_program1 = device->create_shader_program(
        std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>{
            {Shader_type::VERTEX, vertex_shader_code1},
            {Shader_type::FRAGMENT, fragment_shader_code1}
        },
        {
            {"texture0", RHI_uniform_entry<int>::create(0)},
            {"texture1", RHI_uniform_entry<int>::create(1)}
        }
    );

    auto screen_geometry = Geometry::create_plane();

    auto screen_position = screen_geometry->attribute("position");
    auto screen_tex_coord = screen_geometry->attribute("uv");
    auto screen_element = screen_geometry->element_attribute();
    auto screen_vertex_buffer = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        screen_position->unit_data_count(),
        screen_position->data_size(),
        screen_position->data_ptr()
    );
    
    auto screen_tex_coord_buffer = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        screen_tex_coord->unit_data_count(),
        screen_tex_coord->data_size(),
        screen_tex_coord->data_ptr()
    );

    auto screen_element_buffer = device->create_element_buffer(
        Buffer_usage::STATIC,
        screen_element->data_count(),
        screen_element->data_size(),
        screen_element->data_ptr()
    );

    auto screen_geometry1 = device->create_geometry(
        std::unordered_map<unsigned int, std::shared_ptr<RHI_buffer>>{
            {0, screen_vertex_buffer},
            {1, screen_tex_coord_buffer}
        },
        screen_element_buffer
    );

    Clear_state clear_state = Clear_state::enabled();
    clear_state.color_clear_value = glm::vec4(0.1, 0.5, 0.3, 1.0);
    auto renderer = device->create_renderer(clear_state);

    while (window->is_active()) {
        window->on_frame_begin();

        device->check_error();

        renderer->init();
        renderer->clear(frame_buffer);

        renderer->draw(
            shader_program,
            geometry,
            frame_buffer
        );

        color_attachment0->bind_to_unit(0);
        color_attachment1->bind_to_unit(1);
    
        renderer->clear(screen_frame_buffer);

        renderer->draw(
            shader_program1,
            screen_geometry1,
            screen_frame_buffer
        );

        window->on_frame_end();
    }
    
    return 0;
}


