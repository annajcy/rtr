#include "engine/runtime/function/render/object/frame_buffer.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/function/render/object/geometry.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include <memory>

using namespace std;
using namespace rtr;

// 修改顶点数据包含两个不同深度的三角形
std::vector<float> vertices = {
    // 第一个三角形（近处，z = -0.5）
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.0f,  0.5f, -0.5f,

    // 第二个三角形（远处，z = 0.5）
    -0.3f, -0.3f, 0.5f,
     0.7f, -0.3f, 0.5f,
     0.2f,  0.7f, 0.5f
};

std::vector<unsigned int> indices = {
    0, 1, 2,  // 第一个三角形
    3, 4, 5   // 第二个三角形
};

// 顶点着色器（保留深度信息）
const char* vertex_shader_source = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

// 简化片段着色器（不需要颜色输出）
const char* fragment_shader_source = R"(
#version 460 core
void main()
{
    // 空着色器，仅用于深度写入
}
)";

// 后处理着色器（显示深度缓冲）
const char* vertex_shader_source1 = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(vec2(aPos), 0.0, 1.0);
    TexCoords = aUV;
}
)";

const char* fragment_shader_source1 = R"(
#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

//uniform sampler2D depth_texture;
uniform sampler2DArray depth_arr_texture;

void main()
{
    //float depthValue = texture(depth_texture, TexCoords).r;
    float depthValue = texture(depth_arr_texture, vec3(TexCoords, 0)).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}
)";

int main() {
    auto device = std::make_shared<RHI_device_OpenGL>();
    auto window = device->create_window(800, 600, "Depth Buffer Demo");
    auto screen_frame_buffer = device->create_screen_buffer(window);

    // 创建包含两个三角形的几何体
    auto geo = Geometry::create(
        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> {
            {0, Vertex_attribute<float, 3>::create(vertices)}
        },  
        Element_atrribute::create(indices)
    );

    auto geometry = geo->rhi(device);

    // 深度写入着色器
    auto sc0 = Shader_code::create(Shader_type::VERTEX, vertex_shader_source);
    auto sc1 = Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source);
    auto sp = Shader_program::create("depth_shader", {
        {Shader_type::VERTEX, sc0},
        {Shader_type::FRAGMENT, sc1}
    }, {});
    
    auto depth_shader = sp->rhi(device);

    // 深度附件配置
    auto dpa = Texture_2D::create_depth_attachemnt(window->width(), window->height());
    dpa->rhi(device)->set_border_color(glm::vec4(1.0));

    // 帧缓冲配置（仅使用深度附件）
    auto fb = Frame_buffer::create(
        window->width(),
        window->height(),
        std::vector<std::shared_ptr<Texture>>{},  // 无颜色附件
        dpa
    );

    auto frame_buffer = fb->rhi(device);

    auto tex_builder = device->create_texture_builder();
    
    auto tex_depth_arr = Texture_2D_array::create_depth_attachemnt(
        window->width(),
        window->height(),
        1
    );
    
   
    auto texture_array_dep = tex_depth_arr->rhi(device);
    //texture_array_dep->bind_to_unit(3);

    // 后处理着色器
    auto screen_vertex_shader = Shader_code::create(Shader_type::VERTEX, vertex_shader_source1);
    auto screen_fragment_shader = Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source1);
    auto screen_sp = Shader_program::create("screen_shader", {
        {Shader_type::VERTEX, screen_vertex_shader},
        {Shader_type::FRAGMENT, screen_fragment_shader}
    }, {
      //  {"depth_texture", Uniform_entry<int>::create(0)},
        {"depth_arr_texture", Uniform_entry<int>::create(3)}
    });

    auto screen_shader = screen_sp->rhi(device);

    auto sg = Geometry::create_plane();
    auto screen_geometry = sg->rhi(device);

    // 配置渲染状态
    Clear_state clear_state = Clear_state::enabled();
    clear_state.depth_clear_value = 1.0f;  // 初始化为最大深度
    auto renderer = device->create_renderer(clear_state);

    auto pipeline_state = device->create_pipeline_state(Pipeline_state::opaque_pipeline_state());
    pipeline_state->apply();

    while (window->is_active()) {
        window->on_frame_begin();

        // 第一阶段：渲染深度缓冲
        renderer->init();
        renderer->clear(frame_buffer);
        renderer->draw(depth_shader, geometry, frame_buffer);

        // 第二阶段：显示深度缓冲
        //dpa->rhi_resource()->bind_to_unit(0);
        tex_builder->build_texture_2D_array(texture_array_dep, std::vector<std::shared_ptr<RHI_texture>>{
            dpa->rhi(device)
        });

        texture_array_dep->bind_to_unit(3);
        renderer->clear(screen_frame_buffer);
        renderer->draw(screen_shader, screen_geometry, screen_frame_buffer);

        window->on_frame_end();
    }
    
    return 0;
}