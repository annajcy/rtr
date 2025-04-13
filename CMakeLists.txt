cmake_minimum_required(VERSION 3.27)
project(rtr)

set(CMAKE_CXX_STANDARD 20)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_definitions(-DDEBUG)

file(GLOB ASSETS "assets" )
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

option(BUILD_UNIT_TESTS "Disable Unit Test" OFF)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Enable Assimp's built-in zlib" FORCE)

add_subdirectory(ext/glad)
add_subdirectory(ext/assimp)
add_subdirectory(ext/glm)
add_subdirectory(ext/stb_image)
add_subdirectory(ext/imgui)
add_subdirectory(ext/glfw)
add_subdirectory(ext/spdlog)
add_subdirectory(ext/nlohmann_json)

include_directories(build/ext/assimp/include)

include_directories(ext/assimp/include)
include_directories(ext/glad/include)
include_directories(ext/stb_image/include)
include_directories(ext/imgui/include)
include_directories(ext/spdlog/include)
include_directories(ext/glfw/include)
include_directories(ext/nlohmann_json/include)
include_directories(ext/glm)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/engine/*.cpp)

add_executable(rhi_triangle ${SOURCES} example/rhi/triangle.cpp)

target_link_libraries(rhi_triangle 
glfw
glad
assimp
imgui
stb_image
glad
spdlog::spdlog
nlohmann_json::nlohmann_json
${OPENGL_LIBRARIES} 
)

add_executable(rhi_frame_buffer ${SOURCES} example/rhi/frame_buffer.cpp)

target_link_libraries(rhi_frame_buffer 
glfw
glad
assimp
imgui
stb_image
glad
spdlog::spdlog
nlohmann_json::nlohmann_json
${OPENGL_LIBRARIES} 
)

add_executable(rhi_compute ${SOURCES} example/rhi/compute.cpp)

target_link_libraries(rhi_compute 
glfw
glad
assimp
imgui
stb_image
glad
spdlog::spdlog
nlohmann_json::nlohmann_json
${OPENGL_LIBRARIES} 
)


add_executable(main ${SOURCES} example/engine/main.cpp)

target_link_libraries(main
glfw
glad
assimp
imgui
stb_image
glad
spdlog::spdlog
nlohmann_json::nlohmann_json
${OPENGL_LIBRARIES} 
)








