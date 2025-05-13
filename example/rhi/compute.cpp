#include "engine/runtime/function/render/object/memory_buffer.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include <memory>
#include <vector>

using namespace std;
using namespace rtr;

// 计算着色器源代码
const char* compute_shader_source = R"(
#version 460 core
layout(local_size_x = 1) in;


struct UniformParam {
    float scale;
    float offset;
}; 

layout(std140, binding = 0) uniform UniformBlock {
    UniformParam params[4];  // 数组元素自动16字节对齐
};

layout(std430, binding = 1) buffer ResultBuffer {
    float results[];
};

uniform float[4] scales;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    results[idx] = results[idx] * params[idx].scale + params[idx].offset * scales[idx];
}
)";

int main() {
    auto device = std::make_shared<RHI_device_OpenGL>();
    auto window = device->create_window(800, 600, "RTR");

    std::vector<float> initial_data = {1.0f, 2.0f, 3.0f, 4.0f};

    struct UniformData {
        float scale{};
        float offset{};
        // 添加padding保证结构体16字节对齐
        float padding[2];  // 新增填充字段
    };

    std::vector<UniformData> ubo_data(4);

    ubo_data[0] = {1.0f, 0.0f, {}};  // 初始化添加padding
    ubo_data[1] = {2.0f, 1.0f, {}};
    ubo_data[2] = {3.0f, 2.0f, {}};
    ubo_data[3] = {4.0f, 3.0f, {}};

    auto ubos = Uniform_buffer_array<UniformData>::create(ubo_data);
    ubos->link(device);

    auto uniform_buffer = ubos->rhi_resource();

    auto sb = Storage_buffer_array<float>::create(initial_data);

    sb->link(device);

    auto storage_buffer = sb->rhi_resource();

    auto cs = Shader_code::create(Shader_type::COMPUTE, compute_shader_source);

    float scales[] = {1, 2, 3, 4};

    auto csp = Shader_program::create("csp", std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
        {Shader_type::COMPUTE, cs}
    }, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
        {"scales", Uniform_entry_array<float>::create(scales, 4)}
    });

    csp->link(device);

    auto compute_program = csp->rhi_resource();
    
    auto memory_binder = device->create_memory_buffer_binder();
    memory_binder->bind_memory_buffer(uniform_buffer, 0);
    memory_binder->bind_memory_buffer(storage_buffer, 1);

    auto compute_task = device->create_compute_task(compute_program);
    compute_task->dispatch(4, 1, 1);
    compute_task->wait();

    sb->pull_from_rhi();

    std::vector<float> res = sb->data();
    
    cout << "Initial data: " << endl;
    for (auto v : initial_data) cout << v << " ";
    cout << endl;

    cout << "Result data (round1): " << endl;
    for (auto v : res) cout << v << " ";
    cout << endl;

    sb->push_to_rhi();

    compute_task->dispatch(4, 1, 1);
    compute_task->wait();

    sb->pull_from_rhi();
    res = sb->data();
    cout << "Result data (round2) : " << endl;
    for (auto v : res) cout << v << " ";
    cout << endl;

    return 0;
}


