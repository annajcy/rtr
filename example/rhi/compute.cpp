#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_buffer.h"
#include "engine/runtime/rhi/rhi_device.h"
#include "engine/runtime/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/rhi/rhi_shader_program.h"
#include <iostream> // 添加输入输出支持
#include <memory>
#include <vector>

using namespace std;
using namespace rtr;

// 计算着色器源代码
const char* compute_shader_source = R"(
#version 460 core
layout(local_size_x = 1) in;

// 修正1：添加结构体结尾分号
struct UniformParam {
    float scale;
    float offset;
};  // 添加分号

layout(std140, binding = 0) uniform UniformBlock {
    UniformParam params[4];  // 数组元素自动16字节对齐
};

layout(std430, binding = 1) buffer ResultBuffer {
    float results[];
};

uniform float[4] scales;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    // 修正参数访问方式
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
    } ubo_data[4];         // 原数组保持4个元素

    ubo_data[0] = {1.0f, 0.0f, {}};  // 初始化添加padding
    ubo_data[1] = {2.0f, 1.0f, {}};
    ubo_data[2] = {3.0f, 2.0f, {}};
    ubo_data[3] = {4.0f, 3.0f, {}};
    
    auto uniform_buffer = device->create_memory_buffer(
        Buffer_type::UNIFORM,
        Buffer_usage::STATIC,
        sizeof(UniformData) * 4,
        ubo_data
    );

    // 创建存储缓冲
    auto storage_buffer = device->create_memory_buffer(
        Buffer_type::STORAGE, 
        Buffer_usage::DYNAMIC, 
        initial_data.size() * sizeof(float), 
        initial_data.data() 
    );

    auto compute_shader = device->create_shader_code(
        Shader_type::COMPUTE, 
        compute_shader_source
    );

    float scales[] = {1, 2, 3, 4};

    // 创建计算专用着色器程序
    auto compute_program = device->create_shader_program(
        std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
            {Shader_type::COMPUTE, compute_shader}
        },
        {
            {"scales", RHI_uniform_entry_array<float>::create(scales, 4)}
        }
    );

    auto memory_binder = device->create_memory_buffer_binder();
    memory_binder->bind_memory_buffer(uniform_buffer, 0);
    memory_binder->bind_memory_buffer(storage_buffer, 1);

    // 设置计算任务
    auto compute_task = device->create_compute_task(compute_program);
    compute_task->dispatch(4, 1, 1);
    compute_task->wait();

    std::vector<float> res{};

    storage_buffer->map_buffer([&](void* data_ptr){
        float* data = static_cast<float*>(data_ptr);
        for (size_t i = 0; i < initial_data.size(); ++i) {
            res.push_back(data[i]);
        }
    }, RHI_buffer_access_flags{
        true,
        false,
        false
    });
    
    for (auto v : initial_data) cout << v << " ";
    cout << endl;
    for (auto v : res) cout << v << " ";
    cout << endl;

    return 0;
}


