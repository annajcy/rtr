# RTR - Real-Time Rendering Engine

A modern, component-based 3D rendering engine built with C++20 and OpenGL, designed for real-time graphics applications and educational purposes.

## ✨ Features

### Core Engine
- **Component-Based Architecture**: ECS-inspired design with World, Scene, and GameObject hierarchy
- **Modern C++20**: Leveraging latest C++ features for clean, efficient code
- **Cross-Platform**: Built with CMake for easy portability

### Rendering Features
- **Forward Rendering Pipeline**: Optimized forward rendering with support for multiple lights
- **RHI Abstraction Layer**: Hardware abstraction for potential multi-API support
- **Advanced Lighting**: Phong shading model with shadow mapping
- **Material System**: Extensible material system with support for:
  - Phong materials
  - Parallax mapping
  - Texture arrays
- **Post-Processing**: Frame buffer support for post-processing effects
- **Compute Shaders**: OpenGL compute shader support
- **Skybox Rendering**: Environment mapping support

### Component System
- **Camera System**: Flexible camera with orbital controls
- **Light Components**: Point, directional, and spot light support
- **Mesh Renderer**: Efficient mesh rendering with material support
- **Shadow Caster**: Dynamic shadow generation
- **Custom Components**: Easy component extension (rotation, animation, etc.)

### Editor Features
- **ImGui Integration**: Built-in editor panels for:
  - FPS monitoring
  - Material settings
  - Shadow configuration
  - Parallax mapping controls
- **Scene Management**: Runtime scene switching and management

## 📋 Prerequisites

- **C++ Compiler**: GCC 13+ or Clang with C++20 support
- **CMake**: Version 3.14 or higher
- **OpenGL**: Version 4.6 support required
- **Git**: For fetching dependencies

### System Dependencies
The project automatically fetches most dependencies via CMake FetchContent, but you'll need:
- OpenGL development libraries
- X11 development libraries (Linux)
- Basic build tools (make, ninja, etc.)

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
sudo apt-get install libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

**macOS:**
```bash
brew install cmake
# OpenGL is included with Xcode Command Line Tools
xcode-select --install
```

## 🚀 Building

### Clone the Repository
```bash
git clone https://github.com/annajcy/rtr.git
cd rtr
```

### Build Steps
```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make -j$(nproc)

# Built executables will be in the build directory
```

### Build Targets
The project builds multiple executables:
- **Engine Examples**: `mary`, `sponza`, `cubes`
- **RHI Examples**: `rhi_frame_buffer`, `rhi_texture_array`, `rhi_frame_buffer_depth`, `rhi_compute`

## 🎮 Usage

### Running Examples

After building, you can run various examples:

```bash
# Run the Mary model example
./mary

# Run the Sponza scene
./sponza

# Run the cubes demo
./cubes

# Run RHI examples
./rhi_frame_buffer
./rhi_compute
```

### Creating Your Own Scene

```cpp
#include "engine/runtime/runtime.h"
#include "engine/runtime/framework/core/world.h"
#include "engine/runtime/framework/core/scene.h"

using namespace rtr;

int main() {
    // Initialize engine runtime
    Engine_runtime_descriptor descriptor{};
    descriptor.width = 1280;
    descriptor.height = 720;
    descriptor.title = "My RTR Application";
    
    auto runtime = Engine_runtime::create(descriptor);
    
    // Create world and scene
    auto world = World::create("MyWorld");
    auto scene = world->add_scene("MainScene");
    
    // Add game objects, lights, cameras...
    // See examples/ directory for complete examples
    
    runtime->run();
    return 0;
}
```

## 📁 Project Structure

```
rtr/
├── assets/              # Assets (models, textures, shaders)
│   ├── model/          # 3D models (Sponza, etc.)
│   ├── shader/         # GLSL shaders
│   └── image/          # Textures
├── engine/             # Engine source code
│   ├── editor/         # Editor UI panels
│   └── runtime/        # Core runtime
│       ├── framework/  # ECS framework (World, Scene, GameObject)
│       ├── function/   # Systems (Render, Input)
│       ├── platform/   # Platform abstraction (RHI)
│       ├── resource/   # Resource management
│       └── tool/       # Utilities (Logger, Timer)
├── example/            # Example applications
│   ├── engine/         # Engine feature examples
│   ├── rhi/           # Low-level RHI examples
│   └── test/          # Test scenes
├── docs/              # Documentation and diagrams
└── CMakeLists.txt     # Build configuration
```

## 🛠️ Architecture

The engine follows a layered architecture:

1. **Platform Layer (RHI)**: Hardware abstraction for graphics API
2. **Runtime Layer**: Core engine systems and framework
3. **Function Layer**: High-level systems (rendering, input, etc.)
4. **Editor Layer**: Development tools and UI

See `docs/image/arch.jpg` for detailed architecture diagrams.

## 📚 Dependencies

The project uses CMake FetchContent to automatically manage dependencies:

- **[GLFW 3.3.8](https://www.glfw.org/)**: Window and input handling
- **[GLM 0.9.9.8](https://glm.g-truc.net/)**: OpenGL Mathematics
- **[Assimp 5.3.1](https://www.assimp.org/)**: 3D model loading
- **[Dear ImGui 1.90.1](https://github.com/ocornut/imgui)**: Immediate mode GUI
- **[spdlog 1.15.3](https://github.com/gabime/spdlog)**: Fast logging library
- **[nlohmann/json 3.11.3](https://github.com/nlohmann/json)**: JSON parsing
- **[stb_image](https://github.com/nothings/stb)**: Image loading
- **[tinyobjloader 2.0.0rc13](https://github.com/tinyobjloader/tinyobjloader)**: OBJ file parsing
- **[GLAD 0.1.36](https://glad.dav1d.de/)**: OpenGL loader

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

```
Copyright (c) 2025 Ceyang Jin
```

## 🙏 Acknowledgments

- Crytek Sponza model by Frank Meinl
- Games104 Course

For questions or suggestions, please open an issue on GitHub.

---

**Note**: This is an educational rendering engine project. It's designed for learning real-time graphics programming and exploring rendering techniques.

