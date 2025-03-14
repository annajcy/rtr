#pragma once

#include "engine/global/base.h"

namespace rtr {

template <typename ...Args>
class Action {
private:
    std::vector<std::function<void(Args...)>> m_actions{};

public:

    Action() = default;
    Action(const std::vector<std::function<void(Args...)>> &actions) : m_actions(actions) {}

    Action(const std::function<void(Args...)> &action) {
        m_actions.push_back(action);
    }

    void add(const std::function<void(Args...)>& action) {
        m_actions.push_back(action);
    }

    void clear() {
        m_actions.clear();
    }

    void remove(const std::function<void(Args...)> &action) {
        m_actions.erase(std::remove(m_actions.begin(), m_actions.end(), action), m_actions.end());
    }

    void execute(Args... args) {
        for (const auto& action : m_actions) {
            action(args...);
        }
    }
};
    
}

