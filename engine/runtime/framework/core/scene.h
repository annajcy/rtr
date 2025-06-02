#pragma once

#include "engine/runtime/framework/core/game_object.h"
#include "engine/runtime/framework/plugin/model_loader.h"
#include "engine/runtime/function/render/material/shading/phong_material.h"
#include <memory>
#include <string>
#include <vector>

namespace rtr {
    
class Scene {

protected:
    std::string m_name{};
    std::vector<std::shared_ptr<Game_object>> m_game_objects{};
    std::shared_ptr<Skybox> m_skybox{};
    
public:
    Scene(const std::string& name) : m_name(name) {}
    static std::shared_ptr<Scene> create(const std::string& name) {
        return std::make_shared<Scene>(name);
    }

    virtual ~Scene() = default;
    const std::string& name() const { return m_name; }
    void set_skybox(const std::shared_ptr<Skybox>& skybox) { 
        m_skybox = skybox; 
    }
    const std::shared_ptr<Skybox>& skybox() const { return m_skybox; }

    std::shared_ptr<Game_object> add_game_object(const std::shared_ptr<Game_object>& game_object) {
        m_game_objects.push_back(game_object);
        return game_object;
    }

    std::shared_ptr<Game_object> add_game_object(const std::string& name) {
        auto game_object = Game_object::create(name);
        m_game_objects.push_back(game_object);
        return game_object;
    }

    std::shared_ptr<Game_object> add_model(
        const std::string& name,
        const std::shared_ptr<Model>& model,
        const std::shared_ptr<Base_model_loader>& model_loader
    ) {
        std::vector<std::shared_ptr<Game_object>> gos{};
        auto root_go = model_loader->load_model(
            name,
            model,
            gos
        );
        for (auto& go : gos) {
            add_game_object(go);
        }
        return root_go;
    }

    const std::vector<std::shared_ptr<Game_object>>& game_objects() const { return m_game_objects; }
    
    std::shared_ptr<Game_object> get_game_object(const std::string& name) {
        for (auto& game_object : m_game_objects) {
            if (game_object->name() == name) {
                return game_object;
            }
        }
        return nullptr;
    }

    void remove_game_object(const std::string& name) {
        for (auto it = m_game_objects.begin(); it != m_game_objects.end(); ++it) {
            if ((*it)->name() == name) {
                m_game_objects.erase(it);
                return;
            }
        }
    }

    void remove_game_object(const std::shared_ptr<Game_object>& game_object) {
        for (auto it = m_game_objects.begin(); it!= m_game_objects.end(); ++it) {
            if ((*it) == game_object) {
                m_game_objects.erase(it);
                return;
            }
        }
    }

    void clear() {
        m_game_objects.clear();
    }

    void tick(const Logic_tick_context& tick_context) {
        tick_context.logic_swap_data.skybox = m_skybox;

        for (auto& game_object : m_game_objects) {
            game_object->tick(tick_context);
        }
    }

};

};