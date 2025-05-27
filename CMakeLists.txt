cmake_minimum_required(VERSION 3.27)
project(rtr)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DDEBUG)

file(GLOB ASSETS "assets" )
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "Disable JSON tests")
add_subdirectory(ext/nlohmann_json)
include_directories(ext/nlohmann_json/include)

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Enable Assimp's built-in zlib" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable Assimp's tests" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Disable Assimp's tools" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "Disable Assimp's samples" FORCE)
add_subdirectory(ext/assimp)
include_directories(build/ext/assimp/include)
include_directories(ext/assimp/include)

add_subdirectory(ext/glad)
include_directories(ext/glad/include)

add_subdirectory(ext/stb_image)
include_directories(ext/stb_image/include)

add_subdirectory(ext/imgui)
include_directories(ext/imgui/include)

add_subdirectory(ext/spdlog)
include_directories(ext/spdlog/include)

add_subdirectory(ext/glfw)
include_directories(ext/glfw/include)

add_subdirectory(ext/glm)
include_directories(ext/glm)

add_subdirectory(ext/mirrow)
include_directories(ext/mirrow/include)

include_directories(ext/tinyobjectloader)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES 
${CMAKE_SOURCE_DIR}/engine/*.cpp)

# 在包含目录之后定义公共链接库列表
set(COMMON_LIBS
    glfw
    glad      # 移除重复的glad
    assimp
    imgui
    stb_image
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    mirrow
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

add_executable(main ${SOURCES} example/engine/main.cpp)
target_link_libraries(main ${COMMON_LIBS})

add_executable(sponza ${SOURCES} example/engine/sponza.cpp)
target_link_libraries(sponza ${COMMON_LIBS})

add_executable(cubes ${SOURCES} example/engine/cubes.cpp)
target_link_libraries(cubes ${COMMON_LIBS})







