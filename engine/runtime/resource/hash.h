#pragma once

#include <string>
#include <functional> 

namespace rtr {
struct Hash {
    private:
        unsigned long long m_value{};

    public:
        Hash() = default;
        Hash(const Hash&) = default;
        Hash(Hash&&) = default;
        explicit Hash(unsigned long long value) : m_value(value) {} // Made explicit to prevent unintended conversions
        Hash& operator=(const Hash&) = default;
        Hash& operator=(Hash&&) = default;
        bool operator==(const Hash& other) const { return m_value == other.m_value; }
        bool operator!=(const Hash& other) const { return m_value != other.m_value; }

        Hash& operator+=(const Hash& other) {
            // Golden ratio prime 0x9e3779b97f4a7c15
            
            constexpr unsigned long long prime = 0x9e3779b97f4a7c15ULL;
            m_value ^= other.m_value + prime + (m_value << 6) + (m_value >> 2);
            return *this;
        }

        Hash operator+(const Hash& other) const {
            Hash combined = *this;
            combined += other;
            return combined;
        }

        unsigned long long value() const { return m_value; }

        static Hash from_string(const std::string& str) {
            constexpr unsigned long long fnv_prime = 0x100000001b3ULL;
            unsigned long long hash = 0xcbf29ce484222325ULL; // FNV offset basis 

            for (const char& ch : str) {
                hash ^= static_cast<unsigned char>(ch); // Ensure char is treated as unsigned for bitwise operations
                hash *= fnv_prime;
            }
            return Hash(hash);
        }

        static Hash from_raw_data(const unsigned char* data, size_t size) { // Changed unsigned int to size_t for size
            constexpr unsigned long long prime = 0x9E3779B185EECB87ULL;
            unsigned long long hash_val = 0x2D358005CCAE049FULL; // Renamed to avoid conflict with struct name in this scope

            // Process 8-byte blocks 
            const auto* blocks = reinterpret_cast<const unsigned long long*>(data);
            size_t num_blocks = size / 8;
            for (size_t i = 0; i < num_blocks; ++i) {
                hash_val ^= blocks[i];
                hash_val = (hash_val << 13) | (hash_val >> (64 - 13)); // Ensure full 64-bit rotation
                hash_val *= prime;
            }

            const unsigned char* tail = data + num_blocks * 8;
            unsigned long long last = 0;
            size_t remaining_bytes = size % 8;
            for (size_t i = 0; i < remaining_bytes; ++i) {
                last |= static_cast<unsigned long long>(tail[i]) << (i * 8);
            }

            if (remaining_bytes > 0) { // Only apply if there are remaining bytes
                hash_val ^= last;
                hash_val *= prime;
            }

            return Hash(hash_val ^ (hash_val >> 33));
        }
};
} // namespace rtr


namespace std {
    template <>
    struct hash<rtr::Hash> {
        std::size_t operator()(const rtr::Hash& h) const noexcept {
            return std::hash<unsigned long long>{}(h.value());
        }
    };
}