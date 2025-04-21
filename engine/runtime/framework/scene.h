#pragma once

#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/core/skybox.h"
#include "game_object.h"
#include <memory>
#include <vector>

namespace rtr {
    
class Scene : public GUID {

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

    const std::vector<std::shared_ptr<Game_object>>& game_objects() const { return m_game_objects; }
    
    std::shared_ptr<Game_object> get_game_object(const std::string& name) {
        for (auto& game_object : m_game_objects) {
            if (game_object->name() == name) {
                return game_object;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Game_object> get_game_object(GUID guid) {
        for (auto& game_object : m_game_objects) {
            if (game_object->guid() == guid) {
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

    void remove_game_object(GUID guid) {
        for (auto it = m_game_objects.begin(); it!= m_game_objects.end(); ++it) {
            if ((*it)->guid() == guid) {
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

        if (m_skybox != nullptr) {
            tick_context.logic_swap_data.has_skybox = true;
            tick_context.logic_swap_data.skybox = Swap_skybox{
                .skybox_material = m_skybox->material(),
                .skybox_geometry = m_skybox->geometry()
            };
        } else {
            tick_context.logic_swap_data.has_skybox = false;
        }

        for (auto& game_object : m_game_objects) {
            game_object->tick(tick_context);
        }
    }

};

};