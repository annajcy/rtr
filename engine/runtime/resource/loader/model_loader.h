#pragma once

#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/geometric.hpp"

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>

namespace rtr {

struct Model_geometry {
    std::vector<float> positions{};
    std::vector<float> uvs{};
    std::vector<float> normals{};
    std::vector<float> tangents{};
    std::vector<unsigned int> indices{};

    bool has_uvs() const {
        return !uvs.empty();
    }

    bool has_normals() const {
        return !normals.empty();
    }

    bool has_tangents() const {
        return !tangents.empty();
    }
    
    size_t faces_count() const {
        return indices.size() / 3;
    }

    glm::vec3 get_position(
        size_t face_index, 
        size_t vertex_index
    ) const {
        auto index = indices[face_index * 3 + vertex_index];
        return glm::vec3(
            positions[index * 3],
            positions[index * 3 + 1],
            positions[index * 3 + 2]
        );
    }

    glm::vec2 get_uv(
        size_t face_index,
        size_t vertex_index
    ) const {
        if (!has_uvs()) {
            throw std::runtime_error("Model has no uvs");
        }
        auto index = indices[face_index * 3 + vertex_index];
        return glm::vec2(
            uvs[index * 2],
            uvs[index * 2 + 1]
        );
    }

    glm::vec3 get_normal(
        size_t face_index,
        size_t vertex_index
    ) const {
        if (!has_normals()) {
            throw std::runtime_error("Model has no normals");
        }
        auto index = indices[face_index * 3 + vertex_index];
        return glm::vec3(
            normals[index * 3],
            normals[index * 3 + 1],
            normals[index * 3 + 2]
        );
    }

    glm::vec3 get_tangent(
        size_t face_index,
        size_t vertex_index
    ) const {
        if (!has_tangents()) {
            throw std::runtime_error("Model has no tangents");
        }
        auto index = indices[face_index * 3 + vertex_index];
        return glm::vec3(
            tangents[index * 3],
            tangents[index * 3 + 1],
            tangents[index * 3 + 2]
        );
    }
};

struct Model_material {
    glm::vec3 ka{};         // 环境光颜色 (Ambient)
    glm::vec3 kd{};         // 漫反射颜色 (Diffuse)
    glm::vec3 ks{};         // 镜面反射颜色 (Specular)
    glm::vec3 ke{};         // 自发光颜色 (Emissive)
    float shininess{};             // 镜面高光指数 (Shininess)
    float ior{};             // 折射率 (Index of Refraction)
    float alpha{};              // 不透明度 (Dissolve/Opacity)

    // 纹理贴图
    std::shared_ptr<Image> map_albedo{};   // 通常是漫反射贴图 (map_Kd)
    std::shared_ptr<Image> map_specular{}; // 镜面贴图 (map_Ks)
    std::shared_ptr<Image> map_alpha{};    // Alpha/不透明度贴图 (map_d)
    std::shared_ptr<Image> map_normal{};   // 法线贴图 (norm or map_Bump if it's a normal map)
    std::shared_ptr<Image> map_height{};   // 高度贴图 (map_Bump)
};

struct Model_mesh {
    std::shared_ptr<Model_geometry> geometry{};
    unsigned int material_index{};
};

struct Model_node {
    std::vector<std::shared_ptr<Model_mesh>> meshes{};
    std::vector<std::shared_ptr<Model_node>> children{};
    std::weak_ptr<Model_node> parent{};
    glm::mat4 local_model_matrix{1.0f};

    std::shared_ptr<Model_node> parent_node() const {
        return parent.lock();
    }

};

class Model {
protected:
    std::shared_ptr<Model_node> m_root{};
    std::vector<std::shared_ptr<Model_material>> m_materials{};
    std::string m_path{};

public:
    Model(const std::string& path) : m_path(path) {}
    std::shared_ptr<Model_node> root_node() const { return m_root; }
    const std::vector<std::shared_ptr<Model_material>>& materials() const { return m_materials; }

    virtual ~Model() = default;
    virtual std::shared_ptr<Model_node> load() = 0;
};

class Model_assimp : public Model {
private:
    const aiScene* m_scene{};
    Assimp::Importer m_importer{};

public:
    Model_assimp(const std::string& path) : Model(path) {
        m_scene = m_importer.ReadFile(path, 
            aiProcess_Triangulate | 
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace
        );

        if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode) {
            throw std::runtime_error("Failed to load model: " + 
                path + " - Error: " +
                std::string(m_importer.GetErrorString()));
        }
    }

    ~Model_assimp() {
        m_importer.FreeScene();
    }

    const aiScene* scene() const {
        return m_scene;
    }

private:
    std::shared_ptr<Model_node> process_node(
        const aiNode* node, 
        const std::shared_ptr<Model_node>& parent
    ) {
        if (!node) {
            return nullptr;
        }

        std::cout << "Processing node: " << node->mName.C_Str() << std::endl;

        auto model_node = std::make_shared<Model_node>();
        model_node->parent = parent;

        model_node->local_model_matrix = glm::mat4(
            node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
            node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
            node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
            node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4
        );

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            auto mesh = m_scene->mMeshes[node->mMeshes[i]];
            auto model_mesh = process_mesh(mesh);
            model_node->meshes.push_back(model_mesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            auto child = process_node(node->mChildren[i], model_node);
            model_node->children.push_back(child);
        }

        return model_node;
    }

    std::shared_ptr<Model_mesh> process_mesh(const aiMesh* mesh) {
        auto model_mesh = std::make_shared<Model_mesh>();
        model_mesh->geometry = process_geometry(mesh);
        model_mesh->material_index = process_material(mesh);
        return model_mesh;
    }

    std::shared_ptr<Model_geometry> process_geometry(const aiMesh* mesh) {
        auto model_geometry = std::make_shared<Model_geometry>();

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

            model_geometry->positions.push_back(mesh->mVertices[i].x);
            model_geometry->positions.push_back(mesh->mVertices[i].y);
            model_geometry->positions.push_back(mesh->mVertices[i].z);

            if (mesh->mTextureCoords[0]) {
                model_geometry->uvs.push_back(mesh->mTextureCoords[0][i].x);
                model_geometry->uvs.push_back(mesh->mTextureCoords[0][i].y);
            }
            
            if (mesh->mNormals) {
                model_geometry->normals.push_back(mesh->mNormals[i].x);
                model_geometry->normals.push_back(mesh->mNormals[i].y);
                model_geometry->normals.push_back(mesh->mNormals[i].z);
            }

            if (mesh->mTangents) {
                model_geometry->tangents.push_back(mesh->mTangents[i].x);
                model_geometry->tangents.push_back(mesh->mTangents[i].y);
                model_geometry->tangents.push_back(mesh->mTangents[i].z);
            }

        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            auto face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                model_geometry->indices.push_back(face.mIndices[j]);
            }

        }

        return model_geometry;

    }

    std::shared_ptr<Image> load_texture(aiMaterial* material, aiTextureType type, unsigned int index, Image_format format) {
        aiString assimp_path_str{};
        if (material->GetTexture(type, index, &assimp_path_str) == AI_SUCCESS) {
            std::filesystem::path model_file_path(m_path); // m_path 需要可访问
            std::filesystem::path texture_relative_path(assimp_path_str.C_Str());
            std::filesystem::path texture_full_path = model_file_path.parent_path() / texture_relative_path;
            return Image::create(format, texture_full_path.string()); // 或 .u8string()
        }
        return nullptr;
    }

    glm::vec3 process_vec3(
        const char* pKey,
        unsigned int type,
        unsigned int idx,
        aiMaterial* material
    ) {
        aiColor3D color{};
        if (material->Get(pKey, type, idx, color) == AI_SUCCESS) {
            return glm::vec3(color.r, color.g, color.b);
        }
        std::cout << "Failed to get color: " << pKey << std::endl;
        return glm::vec3(0.0f);
    }

    float process_float(
        const char* pKey,
        unsigned int type,
        unsigned int idx,
        aiMaterial* material
    ) {
        float value{};
        if (material->Get(pKey, type, idx, value) == AI_SUCCESS) {
            return value;
        }
        std::cout << "Failed to get float: " << pKey << std::endl;
        return 0.0f;
    }

    int process_int(
        const char* pKey,
        unsigned int type,
        unsigned int idx,
        aiMaterial* material
    ) {
        int value{};
        if (material->Get(pKey, type, idx, value) == AI_SUCCESS) {
            return value;
        }
        std::cout << "Failed to get int: " << pKey << std::endl;
        return 0;
    }

    unsigned int process_material(const aiMesh* mesh) {
        if (mesh->mMaterialIndex >= m_scene->mNumMaterials) {
            std::cout << "Invalid material index: " << mesh->mMaterialIndex << std::endl;
            return 0;
        }
        return mesh->mMaterialIndex;
    }

    void load_materials() {
        for (unsigned int i = 0; i < m_scene->mNumMaterials; i++) {
            auto material = m_scene->mMaterials[i];
            auto model_material = std::make_shared<Model_material>();
            // 加载基础属性
            model_material->ka = process_vec3(AI_MATKEY_COLOR_AMBIENT, material);
            model_material->kd = process_vec3(AI_MATKEY_COLOR_DIFFUSE, material);
            model_material->ks = process_vec3(AI_MATKEY_COLOR_SPECULAR, material);
            model_material->ke = process_vec3(AI_MATKEY_COLOR_EMISSIVE, material);
            model_material->shininess = process_float(AI_MATKEY_SHININESS, material);
            model_material->alpha = process_float(AI_MATKEY_OPACITY, material);
            model_material->ior = process_float(AI_MATKEY_REFRACTI, material);

            // 加载纹理
            model_material->map_albedo = load_texture(material, aiTextureType_DIFFUSE, 0, Image_format::RGB_ALPHA);
            model_material->map_specular = load_texture(material, aiTextureType_SPECULAR, 0, Image_format::RGB_ALPHA);
            model_material->map_normal = load_texture(material, aiTextureType_NORMALS, 0, Image_format::RGB_ALPHA);
            model_material->map_height = load_texture(material, aiTextureType_HEIGHT, 0, Image_format::RGB_ALPHA);
            model_material->map_alpha = load_texture(material, aiTextureType_OPACITY, 0, Image_format::RGB_ALPHA);

            m_materials.push_back(model_material);
        }
    }

public:

    std::shared_ptr<Model_node> load() override {
        load_materials();
        m_root = process_node(m_scene->mRootNode, nullptr);
        return m_root;
    }

    bool is_loaded() const {
        return m_root != nullptr;
    }

    static std::shared_ptr<Model_assimp> create(const std::string& path) {
        return std::make_shared<Model_assimp>(path);
    }

};

}