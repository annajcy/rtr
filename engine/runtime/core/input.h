#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_window.h"


namespace rtr {

class Input {

private:
    std::unordered_map<Key_mod, Key_action> m_key_mods{};
    std::unordered_map<Key_code, Key_action> m_keys{};
    std::unordered_map<Mouse_button, Key_action> m_mouse_buttons{};

    double m_mouse_x{};
    double m_mouse_y{};

    double m_mouse_dx{};
    double m_mouse_dy{};

    double m_mouse_scroll_dx{};
    double m_mouse_scroll_dy{};

public:
    Input(const std::shared_ptr<RHI_window>& window) {
        window->mouse_move_event().add([&](double x, double y) {
            this->update_mouse_position(x, y);
        });

        window->mouse_scroll_event().add([&](double x, double y) {
            this->update_mouse_scroll(x, y);
        });

        window->key_event().add([&](Key_code key, Key_action action, int mod) {
            this->update_key(key, action, mod);
        });

        window->mouse_button_event().add([&](Mouse_button button, Key_action action, int mod) {
            this->update_mouse_button(button, action);
        });

        window->frame_end_event().add([&](RHI_window* window){
            this->reset_deltas();
        });
    }

    ~Input() = default;

    [[nodiscard]] Key_action key_mod(Key_mod mod) const {
        if (m_key_mods.find(mod) == m_key_mods.end()) {
            return Key_action::RELEASE;
        }
        return m_key_mods.at(mod);
    }

    [[nodiscard]] Key_action key(Key_code key) const {
        if (m_keys.find(key) == m_keys.end()) {
            return Key_action::RELEASE;
        }
        return m_keys.at(key);
    }

    [[nodiscard]] Key_action mouse_button(Mouse_button button) const {
        if (m_mouse_buttons.find(button) == m_mouse_buttons.end()) {
            return Key_action::RELEASE;
        }
        return m_mouse_buttons.at(button);
    }

    [[nodiscard]] double mouse_x() const {
        return m_mouse_x;
    }

    [[nodiscard]] double mouse_y() const {
        return m_mouse_y;
    }

    [[nodiscard]] double mouse_dx() const {
        return m_mouse_dx;
    }

    [[nodiscard]] double mouse_dy() const {
        return m_mouse_dy;
    }

    [[nodiscard]] double mouse_scroll_dx() const {
        return m_mouse_scroll_dx;
    }

    [[nodiscard]] double mouse_scroll_dy() const {
        return m_mouse_scroll_dy;
    }
    
    void update_mouse_position(double x, double y) {
        m_mouse_dx = x - m_mouse_x;
        m_mouse_dy = y - m_mouse_y;
        m_mouse_x = x;
        m_mouse_y = y;
    }

    void update_mouse_scroll(double x, double y) {
        m_mouse_scroll_dx = x;
        m_mouse_scroll_dy = y;
    }

    void update_key(Key_code key, Key_action action, int mod) {
        
        for (int i = mod, cnt = 0; i > 0; i >>= 1, cnt ++) {
            if (i & 1) {
                m_key_mods[static_cast<Key_mod>(cnt)] = action;
            }
        }

        m_keys[key] = action;
    }

    void update_mouse_button(Mouse_button button, Key_action action) {
        m_mouse_buttons[button] = action;
    }

    void reset_deltas() {
        m_mouse_dx = 0.0;
        m_mouse_dy = 0.0;
        m_mouse_scroll_dx = 0.0;
        m_mouse_scroll_dy = 0.0;
    }

};

};