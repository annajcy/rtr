cmake_minimum_required(VERSION 3.10)
project(rtr)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DDEBUG)

file(GLOB ASSETS "assets" )
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

add_subdirectory(ext/assimp)
add_subdirectory(ext/glad)
add_subdirectory(ext/glfw)
add_subdirectory(ext/glm)
add_subdirectory(ext/stb_image)
add_subdirectory(ext/imgui)

include_directories(build/ext/assimp/include)

include_directories(ext/assimp/include)
include_directories(ext/glad/include)
include_directories(ext/glfw/include)
include_directories(ext/stb_image/include)
include_directories(ext/imgui/include)
include_directories(ext/glm)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/engine/*.cpp)

add_executable(rtr ${SOURCES} example/test.cpp)

target_link_libraries(rtr 
glfw
glad
assimp
imgui
stb_image
glad
${OPENGL_LIBRARIES} 
)







