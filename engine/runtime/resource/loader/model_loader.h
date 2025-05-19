#pragma once

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "glm/ext/vector_float3.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

namespace rtr {

struct Model_geometry {
   std::vector<float> positions{};
   std::vector<float> uvs{};
   std::vector<float> normals{};
   std::vector<float> tangents{};
   std::vector<unsigned int> indices{};
};

struct Model_material {
    glm::vec3 ka{};         // 环境光颜色 (Ambient)
    glm::vec3 kd{};         // 漫反射颜色 (Diffuse)
    glm::vec3 ks{};         // 镜面反射颜色 (Specular)
    glm::vec3 ke{};         // 自发光颜色 (Emissive)
    float ns{};             // 镜面高光指数 (Shininess)
    float ni{};             // 折射率 (Index of Refraction)
    float d{};              // 不透明度 (Dissolve/Opacity)
    int illum{};            // 光照模型 (Illumination model) - 注意 MTL 是 int, Assimp 也是 int

    // 纹理贴图
    std::shared_ptr<Image> map_Kd{};   // 通常是漫反射贴图 (map_Kd)
    std::shared_ptr<Image> map_Ks{}; // 镜面贴图 (map_Ks)
    std::shared_ptr<Image> map_d{};    // Alpha/不透明度贴图 (map_d)
    std::shared_ptr<Image> map_norm{};   // 法线贴图 (norm or map_Bump if it's a normal map)
    std::shared_ptr<Image> map_Bump{};   // 高度贴图 (map_Bump)
};

struct Model_mesh {
    std::shared_ptr<Model_geometry> geometry{};
    std::shared_ptr<Model_material> material{};
};

struct Model_node {
    std::vector<std::shared_ptr<Model_mesh>> meshes{};
    std::vector<std::shared_ptr<Model_node>> children{};
    std::weak_ptr<Model_node> parent{};
};

class Model {
    std::shared_ptr<Model_node> m_root{};
public:
    Model() : m_root(std::make_shared<Model_node>()) {}
    std::shared_ptr<Model_node> root() const { return m_root; }
};

class Model_assimp {
private:
    const aiScene* m_scene{};

public:
    Model_assimp(const std::string& path) {
        Assimp::Importer importer{};
        m_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);
        if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode) {
            throw std::runtime_error("Failed to load model: " + path);
        }
    }

    ~Model_assimp() {
        if (m_scene) {
            delete m_scene;
        }
    }

    const aiScene* scene() const {
        return m_scene;
    }

};

}