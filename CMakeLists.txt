cmake_minimum_required(VERSION 3.14)
project(rtr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 FetchContent 模块
include(FetchContent)

# FetchContent 优化配置
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

file(GLOB ASSETS "assets" )
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

# 使用 FetchContent 获取第三方库
# nlohmann_json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
set(JSON_BuildTests OFF CACHE INTERNAL "Disable JSON tests")
FetchContent_MakeAvailable(nlohmann_json)

# Assimp
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1
)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "Enable Assimp's built-in zlib" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp's tests" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Disable Assimp's tools" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "Disable Assimp's samples" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Disable Assimp install" FORCE)
FetchContent_MakeAvailable(assimp)

# 为 assimp 禁用特定的编译器警告
if(TARGET assimp)
    target_compile_options(assimp PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-deprecated-copy>
        $<$<CXX_COMPILER_ID:AppleClang>:-Wno-deprecated-copy>
        $<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-copy>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:AppleClang>:-Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:GNU>:-Wno-unused-parameter>
    )
endif()

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Disable spdlog examples" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Disable spdlog tests" FORCE)
FetchContent_MakeAvailable(spdlog)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable GLFW docs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Disable GLFW tests" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable GLFW examples" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)


# tinyobjloader (header-only)
FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG v2.0.0rc13
)
FetchContent_MakeAvailable(tinyobjloader)

# Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.1
)
FetchContent_MakeAvailable(imgui)

# 创建 ImGui 库目标
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw ${OPENGL_LIBRARIES})

# stb (header-only)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# 创建 stb_image 库目标
set(STB_IMAGE_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/stb_image.cpp)
file(WRITE ${STB_IMAGE_SOURCE} 
"#define STB_IMAGE_IMPLEMENTATION
#include \"stb_image.h\"")

add_library(stb_image STATIC ${STB_IMAGE_SOURCE})
target_include_directories(stb_image PUBLIC ${stb_SOURCE_DIR})

# GLAD (OpenGL loader)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
set(GLAD_API "gl=4.6" CACHE STRING "API type/version pairs")
set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")
FetchContent_MakeAvailable(glad)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${glad_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCES 
${CMAKE_SOURCE_DIR}/engine/*.cpp)

# 定义公共链接库列表 - 使用 FetchContent 获取的目标
set(COMMON_LIBS
    glfw
    glad
    assimp
    stb_image
    imgui
    glm::glm
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    ${OPENGL_LIBRARIES}
)

add_executable(rhi_frame_buffer ${SOURCES} example/rhi/frame_buffer.cpp)
target_link_libraries(rhi_frame_buffer ${COMMON_LIBS})

add_executable(rhi_texture_array ${SOURCES} example/rhi/texture_array.cpp)
target_link_libraries(rhi_texture_array ${COMMON_LIBS})

add_executable(rhi_frame_buffer_depth ${SOURCES} example/rhi/frame_buffer_depth.cpp)
target_link_libraries(rhi_frame_buffer_depth ${COMMON_LIBS})

add_executable(rhi_compute ${SOURCES} example/rhi/compute.cpp)
target_link_libraries(rhi_compute ${COMMON_LIBS})

add_executable(mary ${SOURCES} example/engine/mary.cpp)
target_link_libraries(mary ${COMMON_LIBS})

add_executable(sponza ${SOURCES} example/engine/sponza.cpp)
target_link_libraries(sponza ${COMMON_LIBS})

add_executable(cubes ${SOURCES} example/engine/cubes.cpp)
target_link_libraries(cubes ${COMMON_LIBS})

