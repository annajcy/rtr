# RTR 实时渲染引擎

## 简介
RTR 是一个实时渲染引擎，它可以在运行时生成场景并渲染出图像。
目前项目的整体体量较小，还停留在玩具级别的状态，但也因为其轻量化的特点，比较适合用于学习和研究，或者实验一些实时渲染的算法。

## 系统架构

### 核心模块组成

#### 1. 核心层 (Core)
- **GameObject**：游戏对象基类，支持组件挂载
- **Component系统**：
  - Transform组件（Node）
  - Camera组件（支持透视/正交相机）
  - Light组件（平行光/点光源/聚光灯）
  - MeshRenderer组件
  - 自定义组件（如RotateComponent）
- **场景管理**：
  - Scene（场景管理多个GameObject）
  - World（世界管理多个Scene）

#### 2. 功能层 (Function)
- **渲染系统**：
  - 前向渲染管线（ForwardRenderPipeline）
  - 多Pass架构（ShadowPass/MainPass/PostprocessPass）
  - 阴影系统（DirectionalLightShadowCaster）
  - 天空盒系统（支持立方体贴图和球形贴图）
- **输入系统**：处理键盘/鼠标输入
- **窗口系统**：基于GLFW的窗口管理

#### 3. 资源管理层 (Resource)
- **资源加载器**：
  - ModelLoader（模型加载）
  - ImageLoader（纹理加载）
- **资源类型**：
  - Material（Phong材质/PBR材质）
  - Texture（支持法线贴图/高度图）
  - Shader（GLSL着色器管理）

#### 4. 运行时层 (Runtime)
- 引擎主循环（EngineRuntime）
- 上下文管理：
  - 逻辑上下文（LogicTickContext）
  - 渲染上下文（RenderTickContext）
- 内存管理：
  - 统一缓冲区（UniformBuffer）
  - 资源管理器（ResourceManager）

#### 5. 工具层 (Editor)
- 编辑器系统（支持参数调节面板）
- 调试工具：
  - 帧缓冲查看
  - 材质参数实时调节

### 架构示意图
![系统架构](docs/image/arch_fig.jpg)

### 关键依赖
- 图形API：OpenGL Core Profile
- 第三方库：
  - GLFW（窗口管理）
  - GLAD（OpenGL加载）
  - GLM（数学库）
  - Assimp（模型加载）
  - stb_image（图像加载）


