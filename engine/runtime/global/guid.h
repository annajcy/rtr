#pragma once

#include "serialization.h"

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
    unsigned int m_guid{};
public:
    GUID() : m_guid(GUID_generator::generate_GUID()) {}
    GUID(unsigned int guid) : m_guid(guid) {}
    unsigned int guid() const { return m_guid; }
    bool operator==(const GUID& other) const {
        return m_guid == other.m_guid;
    }
};

}