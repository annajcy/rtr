#pragma once

#include "engine/global/base.h"
#include <vector>

namespace rtr {

enum class RHI_resource_type {
    BUFFER,
    TEXTURE,
    SHADER_CODE,
    SHADER_PROGRAM,
    GEOMETRY,
    FRAME_BUFFER
};

class RHI_resource : public GUID {
protected:
    RHI_resource_type m_type{};
public:

    RHI_resource(RHI_resource_type type) : GUID(), m_type(type) {}
    virtual ~RHI_resource() = default;
    virtual const void* native_handle() const = 0;
    virtual RHI_resource_type resource_type() const { return m_type; }
};

//key: GUID

class RHI_resource_manager {
public:
    inline static std::unordered_map<unsigned int, RHI_resource*> resource_map{};
    inline static std::unordered_map<unsigned int, std::unordered_set<unsigned int>> dependency_map{};
    inline static std::unordered_map<unsigned int, std::unordered_set<unsigned int>> reverse_dependency_map{};

    static void add_resource(RHI_resource* resource) {
        resource_map[resource->guid()] = resource;
    }

    static void add_resource_with_dependency(RHI_resource* resource, const std::vector<unsigned int>& dependency_ids) {
        resource_map[resource->guid()] = resource;
        for (auto& dependency_id : dependency_ids) {
            add_dependency(resource->guid(), dependency_id);
        }
    }

    static void remove_resource(unsigned int id) {

        if (!resource_map.contains(id)) {
            return;
        }

        delete resource_map[id];
        resource_map.erase(id);

        std::unordered_set<unsigned int> dependency_ids = dependency_map[id];
        std::unordered_set<unsigned int> reverse_dependency_ids = reverse_dependency_map[id];

        for (auto& dependency_id : dependency_ids) {
            remove_dependency(dependency_id, id);
        }

        for (auto& reverse_dependency_id : reverse_dependency_ids) {
            remove_dependency(id, reverse_dependency_id);
        }

        dependency_map.erase(id);
        reverse_dependency_map.erase(id);
    }

    static RHI_resource* get_resource(unsigned int id) {
        if (!resource_map.contains(id)) {
            return nullptr;
        }   
        return resource_map[id];
    }

    template<typename T>
    static T* get_resource(unsigned int id) {
        if (!resource_map.contains(id)) {
            return nullptr;
        }
        return static_cast<T*>(resource_map[id]);
    }

    static const void* native_handle(unsigned int id) {
        if (!resource_map.contains(id)) {
            return nullptr;
        }
        return resource_map[id]->native_handle();
    }

    template<typename T>
    static const T* native_handle(unsigned int id) {
        if (!resource_map.contains(id)) {
            return nullptr;
        }
        return reinterpret_cast<const T*>(resource_map[id]->native_handle());
    }

    static unsigned int native_handle_uint(unsigned int id) {
        if (!resource_map.contains(id)) {
            return 0;
        }
        return *(reinterpret_cast<const unsigned int*>(resource_map[id]->native_handle()));
    }

    static void add_dependency(unsigned int id, unsigned int dependency_id) {
        dependency_map[id].insert(dependency_id);
        reverse_dependency_map[dependency_id].insert(id);
    }

    static void remove_dependency(unsigned int id, unsigned int dependency_id) {
        dependency_map[id].erase(dependency_id);
        reverse_dependency_map[dependency_id].erase(id);
        if (reverse_dependency_map[dependency_id].size() == 0) {
            remove_resource(dependency_id);
        }
    }
  
};

}