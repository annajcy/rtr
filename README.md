# RTR 实时渲染引擎

## 简介
RTR 是一个实时3D图形渲染引擎。

## Engine Runtime系统架构

每一层架构的设计相对独立，层与层之间具有单向的调用关系，上层的实现仅调用下层提供的接口。
下图展示了RTR的系统架构。
![系统架构](docs/image/arch.jpg)

RTR的分层架构由底层到上层依次为：
- 工具层
- 资源层
- 平台层
- 功能层
- 上下文层
- 框架层

接下来将详细介绍每一层的设计和实现。

# 工具层

## 概述
工具层主要用于提供一些通用的底层工具类和辅助函数。
第三方库也将会于工具层接入RTR Runtime，工具层将会对其进行封装和抽象，提供统一的接口。
以下是工具层的主要模块

## 设计模式库
RTR Runtime时常会使用某些设计模式，如单例模式、事件中心模式等。工具层提供了这些设计模式的实现，使得其他模块可以方便地使用这些设计模式。

**单例模式实现：**
```cpp
template<typename T>
class Singleton {
public:
    static T& get() {
        static T instance;
        return instance;
    }
    // ... 禁用拷贝和移动构造 ...
};
```

**事件中心模式：**
```cpp
template <typename ...Args>
class Event {
public:
    void add(const std::function<void(Args...)>& action);
    void execute(Args... args); // 触发所有绑定事件
};

template <typename...Args>
class Event_center {
public:
    void register_event(const std::string& event_name, Event<Args...> event);
    void trigger_event(const std::string& event_name, Args... args); // 触发指定事件
};
```

## 数学库
RTR Runtime使用GLM作为数学库，GLM提供了丰富的数学函数和数据结构，用于处理3D图形渲染中的各种数学运算。
另外，RTR Runtime 的数学库还提供了一些常用的计算几何算法，如射线与三角形的交点计算、AABB包围盒的计算，以及求凸包的算法等。

**几何结构示例：**
```cpp
struct Bouding_box {
    glm::vec3 min{};
    glm::vec3 max{};
    
    bool overlap(const Bouding_box& other) const; // 碰撞检测
    glm::vec3 center() const; // 获取包围盒中心
};

struct Triangle {
    glm::vec3 normal() const; // 计算面法线
    float area() const; // 计算三角形面积
};
```

## 日志库
RTR Runtime使用spdlog作为日志库，spdlog提供了灵活的日志输出方式，可以根据需要选择不同的日志级别和输出方式。
RTR Runtime中的日志输出可以通过spdlog提供的接口进行配置和控制。

**日志系统特性：**
```cpp
class Logging_system {
public:
    enum class Level { debug, info, warn, error, fatal };
    
    template<typename... TARGS>
    void log(Level level, std::format_string<TARGS...> fmt, TARGS&&... args);
    
    // 使用示例：
    // Log_sys::get().log(Level::info, "Camera position: {}", camera_pos);
};
```

## 序列化库
RTR Runtime使用nlohmann_json作为序列化库，nlohmann_json提供了简单易用的序列化和反序列化接口，可以方便地将数据结构序列化为JSON格式，或者将JSON格式反序列化为数据结构。序列化库使得数据持久化成为可能。

**序列化示例：**
```cpp
// GLM向量序列化
void to_json(nlohmann::json& j, const glm::vec3& v) {
    j = nlohmann::json::array({v.x, v.y, v.z});
}

// 使用方式：
// auto json_str = Serialization_manager::serialize(object);
// auto obj = Serialization_manager::deserialize<MyClass>(json_str);
```

## 计时器库
RTR Runtime使用std::chrono作为计时器库，std::chrono提供了高精度的计时器功能，可以用于测量程序的执行时间。计时器库使得性能分析和优化成为可能。

**计时器功能：**
```cpp
class Timer {
public:
    void pause();  // 暂停计时
    void resume(); // 恢复计时
    float elapsed_ms() const; // 获取毫秒级耗时
    std::string format() const; // 自动单位转换
};
```

## 枚举和宏
RTR Runtime使用枚举和宏来定义一些常用的常量和类型，如错误码、日志级别、渲染API等。枚举和宏使得代码更加清晰和易于维护。

**关键枚举定义：**
```cpp
enum class Key_code { // 键盘按键枚举
    W = 87,
    A = 65,
    S = 83,
    D = 68,
    // ... 其他按键定义 ...
};

enum class Buffer_type { // 显存缓冲类型
    VERTEX,
    ELEMENT,
    UNIFORM
};
```

# 资源层

## 概述
资源层主要用于提供一些通用的资源管理类和接口。
资源层的设计主要是为了实现跨平台的资源管理，使得RTR Runtime可以在不同的平台上运行。
以下是资源层的主要模块

## 文件服务
`File_service`类封装文件系统操作，提供跨平台的资源存取能力

- 核心功能
  - 设置/获取根目录路径
  - 文件读写操作（支持文本文件）
  - 目录创建与遍历
  - 文件复制/移动/删除
  - 构建文件树结构

- 代码结构
  ```cpp
  class File_service {
  public:
      bool add_text_file(const string& path, const string& content); // 添加文本文件
      bool read_text_file(const string& path, string& content); // 读取文本文件
      vector<string> list_files(const string& dir); // 列出目录文件
      shared_ptr<File_node> build_file_tree(); // 构建文件树
      // ... 其他文件操作接口
  };
  ```

## 资源管理器
`Resource_manager`模板类实现泛型资源管理

- 设计要点
  - 使用unordered_map进行资源缓存
  - 支持类型安全的资源获取(dynamic_pointer_cast)
  - 提供资源存在性检查接口
  - 支持批量清除资源

- 类继承关系
  ```mermaid
  classDiagram
    Resource_manager <|-- Texture_manager
    Resource_manager <|-- Model_manager
    Resource_manager: +get()
    Resource_manager: +add()
    Resource_manager: +remove()
  ```

## 加载器系统
通过Loader体系实现不同类型资源的加载

1. **文本加载器**
   ```cpp
   class Text_loader {
   public:
       static shared_ptr<Text> load_from_path(const string& path);
   };
   ```

2. **图像加载器**
   ```cpp
   class Image_loader {
   public:
       static shared_ptr<Image> load_from_path(Image_format, const string& path);
       static shared_ptr<Image> load_from_data(Image_format, const uchar* data, uint size);
   };
   ```

3. **模型加载器**
   ```cpp
   class Model_loader {
   public:
       static shared_ptr<Model> load(const string& path);
   };
   ```

## 哈希系统
`Hash`类提供资源完整性校验和快速比对

- 哈希算法特性
  - 支持字符串和二进制数据哈希
  - 使用FNV-1a算法和黄金比例混合哈希
  - 提供哈希值组合运算
  - 支持哈希值比对运算符

```cpp
Hash h1 = Hash::from_string("texture_diffuse");
Hash h2 = Hash::from_raw_data(buffer, size);
Hash combined = h1 + h2;
```



# 平台层

## 平台层概述

RTR Runtime的平台层设计旨在提供跨平台的支持，使得开发者可以基于不同的图形API(OpenGL/DirectX12/Vulkan)使用RTR Runtime。

平台层的设计主要包括以下两个方面：

- 平台抽象：通过平台接口层提供统一的平台抽象，使得上层模块可以在不同的平台上运行。也就是下文提到的RHI（Render Hardware Interface）。

- 平台适配：通过平台适配层提供不同平台的适配实现，使得平台层可以在不同的平台上运行。也就是对RHI的具体实现。

平台接口层和平台适配层的设计主要是为了实现跨平台的支持，使得上层模块可以在不同的平台上运行。
在本项目中，RHI接口仅实现了OpenGL的版本，对于其他的平台，需要自行实现RHI接口。

## RHI核心模块

### 设备管理（Device）

#### 跨平台GPU设备抽象
`RHI_device`是一个跨平台的GPU设备抽象，提供了统一的接口，使得上层模块可以在不同的平台上运行。
`RHI_device`的设计主要包括以下几个方面：
- 设备管理：提供设备创建、销毁、配置等接口。
- 资源管理：提供资源创建、销毁、配置等接口。
- 上下文管理：提供上下文创建、销毁、配置等接口。

#### 典型使用流程
  ```cpp
  // 1. 创建设备实例
  auto device = RHI_device_OpenGL::create();
  
  // 2. 创建渲染窗口
  auto window = device->create_window(1280, 720, "RTR Engine");
  
  // 3. 创建顶点/索引缓冲区
  auto vbo = device->create_vertex_buffer(...);
  auto ebo = device->create_element_buffer(...);
  
  // 4. 组装几何体
  auto geometry = device->create_geometry({ {0, vbo} }, ebo);
  
  // 5. 创建着色器程序
  auto shader_program = device->create_shader_program(...);
  ```

### 窗口系统
#### 跨平台窗口抽象
`RHI_window`类作为渲染硬件接口的窗口抽象基类，提供统一的跨平台窗口操作接口。主要功能包括：

- 窗口属性管理
  ```cpp
  const int& width();      // 获取窗口宽度
  const int& height();     // 获取窗口高度
  const std::string& title(); // 获取窗口标题
  ```

- 生命周期控制
  ```cpp
  virtual bool is_active() = 0;  // 窗口激活状态
  virtual void deactivate() = 0; // 关闭窗口
  ```

- 视口设置
  ```cpp
  virtual void set_viewport(int x, int y, int width, int height) = 0;
  ```

#### OpenGL上下文管理
通过继承实现具体平台的上下文管理，核心方法：
```cpp
void on_frame_begin() {
    poll_events();     // 处理系统事件
    swap_buffers();    // 交换前后缓冲
    m_frame_begin_event.execute(this); // 触发帧开始事件
}

void on_frame_end() {
    m_frame_end_event.execute(this); // 触发帧结束事件
}
```
#### 输入事件处理流程
采用事件委托机制实现输入处理：

```cpp
// 事件类型定义
using Window_resize_event = Event<int, int>;
using Key_event = Event<Key_code, Key_action, unsigned int>;

// 事件注册接口
Window_resize_event& window_resize_event(); 
Key_event& key_event();

// 典型事件处理示例
Key_event m_key_event{[&](Key_code key_code, Key_action action, unsigned int repeat) {
    if (key_code == Key_code::ESCAPE && action == Key_action::PRESS) {
        this->deactivate(); // ESC键关闭窗口
    }
}};
```

#### 实现特性
1. **ImGui集成支持**
   ```cpp
   std::shared_ptr<RHI_imgui>& imgui(); // 获取ImGui上下文
   ```

2. **帧事件系统**
   ```cpp
   Event<RHI_window*>& frame_begin_event(); // 帧开始事件
   Event<RHI_window*>& frame_end_event();   // 帧结束事件
   ```

3. **多平台扩展**
   通过继承实现不同图形API的窗口：
   ```cpp
   class RHI_window_OpenGL : public RHI_window {
       // OpenGL具体实现
   };
   
   class RHI_window_Vulkan : public RHI_window {
       // Vulkan具体实现
   };
   ```

#### 使用示例
```cpp
// 创建窗口
auto window = device->create_window(1280, 720, "RTR Engine");

// 注册窗口调整事件
window->window_resize_event().add_listener([](int w, int h) {
    std::cout << "New size: " << w << "x" << h << std::endl;
});

// 主循环
while(window->is_active()) {
    window->on_frame_begin();
    // 渲染逻辑...
    window->on_frame_end();
}
```

### 2.3 资源管理

#### 2.3.1 缓冲区（Buffer）
- 顶点/索引/存储缓冲区
- 内存映射机制

#### 2.3.2 着色器（Shader）
- 着色器代码编译流程
- Uniform管理策略
- 跨平台着色器程序

#### 2.3.3 纹理系统
- 2D/立方体贴图/数组纹理
- Mipmap生成策略
- 帧缓冲附件管理

## 3. 渲染管线

### 3.1 几何体抽象
- 顶点格式描述
- VAO管理策略
- 实例化渲染支持

### 3.2 状态管理
- 混合/深度测试/模板测试
- 多边形偏移配置
- 裁剪状态管理

## 4. 工具模块

### 4.1 ImGui集成
- 平台抽象接口
- 事件处理集成
- 渲染后端对接

### 4.2 计算管线
- 计算着色器分派
- 内存屏障控制
- 异步计算支持

## 5. 全局资源管理
- 设备单例管理
- 渲染器生命周期
- 纹理构建器模式
