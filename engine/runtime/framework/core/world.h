#pragma once
#include "engine/runtime/framework/core/scene.h"
#include <memory>
#include <string>
#include <vector>

namespace rtr {

class World {
protected:
    std::string m_name{};
    std::vector<std::shared_ptr<Scene>> m_scenes{};
    unsigned int m_current_scene_index{};

public:
    World(const std::string& name) : m_name(name) {}
    static std::shared_ptr<World> create(const std::string& name) {
        return std::make_shared<World>(name);
    }

    virtual ~World() = default;
    const std::string& name() const { return m_name; }

    std::shared_ptr<Scene> add_scene(const std::shared_ptr<Scene>& scene) {
        m_scenes.push_back(scene);
        return scene;
    }

    std::shared_ptr<Scene> add_scene(const std::string& name) {
        auto scene = Scene::create(name);
        m_scenes.push_back(scene);
        return scene;
    }

    const std::vector<std::shared_ptr<Scene>>& scenes() const { return m_scenes; }

    std::shared_ptr<Scene> get_scene(const std::string& name) {
        for (auto& scene : m_scenes) {
            if (scene->name() == name) {
                return scene;
            }
        }
        return nullptr;
    }

    void remove_scene(const std::string& name) {
        for (auto it = m_scenes.begin(); it != m_scenes.end(); ++it) {
            if ((*it)->name() == name) {
                m_scenes.erase(it);
                return;
            }
        }
    }

    void set_current_scene(const std::shared_ptr<Scene>& scene) {
        unsigned int index = 0;
        for (auto& s : m_scenes) {
            if (s == scene) {
                m_current_scene_index = index;
                return;
            }
            index ++;
        }
    }

    void set_current_scene(const std::string& name) {
        unsigned int index = 0;
        for (auto& scene : m_scenes) {
            if (scene->name() == name) {
                m_current_scene_index = index;
                return;
            }
            index ++;
        }
    }

    std::shared_ptr<Scene> current_scene() {
        return m_scenes[m_current_scene_index];
    }

    unsigned int current_scene_index() const { return m_current_scene_index; }
    void set_current_scene_index(unsigned int index) { m_current_scene_index = index; }
    void next_scene() {
        m_current_scene_index = (m_current_scene_index + 1) % m_scenes.size();
    }
    void previous_scene() {
        m_current_scene_index = (m_current_scene_index - 1) % m_scenes.size();
    }

    void tick(const Logic_tick_context& tick_context) {
        current_scene()->tick(tick_context);
    }
    
};

};