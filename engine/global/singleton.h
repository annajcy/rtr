#pragma once

#include "engine/global/base.h"

namespace rtr {

template<typename T>
class Singleton {
public:
    static std::shared_ptr<T>& get_instance();
    Singleton();
    virtual ~Singleton();
private:
    static std::shared_ptr<T> m_instance;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::m_instance{};

template<typename T>
Singleton<T>::Singleton() = default;

template<typename T>
Singleton<T>::~Singleton() = default;

template<typename T>
std::shared_ptr<T>& Singleton<T>::get_instance() {
    if (!m_instance) {
        m_instance = std::make_unique<T>();
    }
    return m_instance;
}
    
}









