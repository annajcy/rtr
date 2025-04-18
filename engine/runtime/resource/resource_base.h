#pragma once

#include "engine/runtime/global/base.h" 

namespace rtr {

enum class Resource_type {
    MODEL,
    IMAGE,
    TEXT,
    SHADER,
    MATERIAL,
    MESH,
    TEXTURE,
    UNKNOWN
};

struct Hash {
private:
    unsigned long long m_value{};

public:
    Hash() = default;
    Hash(const Hash&) = default;
    Hash(Hash&&) = default;
    Hash(unsigned long long value) : m_value(value) {}
    Hash& operator=(const Hash&) = default;
    Hash& operator=(Hash&&) = default;
    bool operator==(const Hash& other) const { return m_value == other.m_value; }
    bool operator!=(const Hash& other) const { return m_value != other.m_value; }

    Hash& operator+=(const Hash& other) {
        // 黄金比例素数 0x9e3779b97f4a7c15
        constexpr unsigned long long prime = 0x9e3779b97f4a7c15ULL;
        m_value ^= other.m_value + prime + (m_value << 6) + (m_value >> 2);
        return *this;
    }

    Hash operator+(const Hash& other) const {
        Hash combined = *this;
        combined += other;
        return combined;
    }

    operator unsigned long long() const { return m_value; }

    static Hash from_string(const std::string& str) {
        constexpr unsigned long long fnv_prime = 0x100000001b3ULL;
        unsigned long long hash = 0xcbf29ce484222325ULL; // FNV偏移基础值
        
        for (const char& ch : str) {
            hash ^= static_cast<uint8_t>(ch);
            hash *= fnv_prime;
        }
        return Hash(hash);
    }

    static Hash from_raw_data(const unsigned char* data, unsigned int size) {
        constexpr unsigned long long prime = 0x9E3779B185EECB87ULL;
        unsigned long long hash = 0x2D358005CCAE049FULL;

        // 每次处理8字节
        const auto* blocks = reinterpret_cast<const unsigned long long*>(data);
        for (unsigned i = 0; i < size / 8; ++i) {
            hash ^= blocks[i];
            hash = (hash << 13) | (hash >> 51);
            hash *= prime;
        }

        // 处理剩余字节（1-7字节）
        const unsigned char* tail = data + (size / 8) * 8;
        unsigned long long last = 0;
        for (unsigned i = 0; i < size % 8; ++i) {
            last |= static_cast<unsigned long long>(tail[i]) << (i * 8);
        }
        hash ^= last;
        hash *= prime;

        return Hash(hash ^ (hash >> 33));
    }

};

struct Resource_base {
    Resource_type type{};
    Hash hash{};

    Resource_base(Resource_type type, const Hash& hash) : type(type), hash(hash) {}

    virtual ~Resource_base() = default;
};


};