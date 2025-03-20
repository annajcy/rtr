#pragma once

namespace rtr {

class GUID_generator {
private:
    static inline unsigned int m_id = 0;

public:
    static unsigned int generate_GUID() {
        return m_id ++;
    }
};

class GUID {
private:
    unsigned int m_id{};
public:
    GUID() : m_id(GUID_generator::generate_GUID()) {}
    GUID(unsigned int id) : m_id(id) {}
    unsigned int id() const { return m_id; }
    bool operator==(const GUID& other) const {
        return m_id == other.m_id;
    }
};

}