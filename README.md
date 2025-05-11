# RTR Rendering Engine

RTR is a C++ based 3D rendering engine designed with a focus on modularity, extensibility, and cross-platform capabilities. It provides a user-friendly API for developers to create and manage 3D scenes and rendering processes. The engine is broadly divided into a **Runtime** core and an **Editor** for scene manipulation and debugging.

## Key Features

* **Layered Architecture:** Ensures modularity, maintainability, and ease of testing. Layers have clear responsibilities and unidirectional dependencies.
* **Cross-Platform Support:** Achieved via a Render Hardware Interface (RHI) abstracting underlying graphics APIs (OpenGL implemented; DirectX 12, Vulkan planned).
* **Runtime & Editor:**
    * **Runtime:** The core engine responsible for loading, processing, rendering scenes, and handling user input on end-user devices.
    * **Editor:** An ImGui-based tool for real-time scene editing, debugging, and parameter tweaking of the Runtime.
* **Component-Based Scene System:** Utilizes GameObjects and Components (`Node_component`, `Camera_component`, `Light_component`, `Mesh_renderer_component`, etc.) for flexible scene construction and behavior definition.
* **Comprehensive Resource Management:** Includes file services, a dependency-aware resource manager, and loaders for text and images (using stb_image).
* **Robust Tooling:**
    * Design pattern implementations (Singleton, Event Center).
    * Math library (GLM) with added computational geometry algorithms.
    * Logging (spdlog).
    * Serialization (nlohmann_json).
    * High-precision timers (std::chrono).
* **Advanced Rendering Capabilities:**
    * **Dynamic Shader Variant System:** Allows generation of shader permutations based on features (e.g., shadows, normal maps) to optimize rendering.
    * **Phong/Blinn-Phong Lighting Model:** For realistic material appearances with ambient, diffuse, and specular components.
    * **Material System:** Supports various texture maps including albedo, specular, normal, alpha, and height (parallax) maps.
    * **Shadow Mapping:** Implements Percentage Closer Filtering (PCF) and Percentage Closer Soft Shadows (PCSS) for soft, realistic shadows, with potential for Cascaded Shadow Maps (CSM).
    * **Parallax Occlusion Mapping (POM):** For enhanced surface detail and depth perception.
    * **Skybox Rendering:** Supports both cubemap and spherical skybox textures.
    * **Post-Processing:** Includes effects like Gamma Correction.
* **Modular Render Pipeline:** The rendering process is broken down into configurable `Render_pass` stages (e.g., Shadow Pass, Main Pass, Post-process Pass) orchestrated by a `Render_pipeline` (e.g., `Forward_render_pipeline`).
* **Efficient Data Management:** Utilizes Uniform Buffer Objects (UBOs) for scene-wide data transfer to shaders and a clear context swapping mechanism (`Swap_data`, `Tick_context`) between logic and rendering.

## Architecture Overview

### Runtime Architecture

The Runtime is built upon a layered design:

1.  **Backend Layers (Foundation):**
    * **Tool Layer:** Provides low-level utilities, math functions, logging, serialization, and third-party library abstractions.
    * **Resource Layer:** Manages asset loading (textures, models, shaders), storage, and dependencies. Includes `File_service`, `Resource_manager_dep`, and various loaders.
    * **Platform Layer (RHI):** Abstracts graphics APIs (OpenGL, DirectX, Vulkan). It offers:
        * `RHI_functions`: API for managing GPU rendering capabilities, context, and resource creation/destruction.
        * `RHI_resources`: Object-oriented representation of GPU resources (buffers, textures, shaders, frame buffers). The `RHI_linker<T>` template links these to the frontend.

2.  **Frontend Layers (Application-Facing):**
    * **Function Layer:** Implements core engine functionalities like the Input System and the highly modular Render System. The Render System uses:
        * `Render_object`: Base for all renderable assets.
        * `Render_pass`: Represents a distinct rendering stage (e.g., shadow generation, scene shading).
        * `Render_pipeline`: Orchestrates multiple `Render_pass` instances.
        * Frontend resource wrappers (e.g., `Geometry`, `Material`, `Shader_program`, `Texture`) that often link to RHI resources.
    * **Context Swap Layer:** Defines how data (via `Swap_data`) is exchanged between the Framework and Function layers during each `Tick_context`.
    * **Framework Layer:** Provides high-level APIs for users to define and interact with the virtual world. Core concepts include:
        * `Component`: Individual properties or behaviors.
        * `Component_list`: Manages components for an entity.
        * `Game_object`: Entities in the scene.
        * `Scene`: A container for `Game_object`s, also managing global elements like cameras and skyboxes.
        * `World`: Manages multiple `Scene`s.

### Editor Architecture

The Editor is built on top of the Runtime, leveraging an `RHI_ImGui` driver.
* **Central Editor Class:** Manages a collection of `Panel` instances.
* **Panel System:** Each `Panel` (e.g., `Shadow_settings_panel`, `Phong_material_settings_panel`) is responsible for displaying and modifying a specific set of engine parameters or data in real-time.
* **Decoupled Design:** The editor UI is separated from engine logic, allowing for dynamic inspection and modification of a running `Engine_runtime` instance.

## Rendering Algorithms Implemented

* **Shadows:** PCF and PCSS for soft shadow rendering.
* **Lighting:** Phong/Blinn-Phong model with support for directional, point, and spot lights.
* **Material Expression:**
    * Alpha Mapping for transparency.
    * Normal Mapping for detailed surface geometry.
    * Parallax Occlusion Mapping (POM) for enhanced depth and self-occlusion effects.
* **Post-Processing:** Gamma Correction for accurate color display.

## Scene Test Snapshot

The engine has been tested with scenes featuring:
* Resolution: ~3K
* Triangle Count: ~100k
* Multiple light sources (directional, point, spot)
* PCF + PCSS shadows (20 Poisson disk samples)
* Parallax mapping (10 layers)
* Achieved FPS: 30+

This RTR Rendering Engine aims to provide a solid foundation for developing 3D applications and games, emphasizing flexibility and modern rendering techniques.