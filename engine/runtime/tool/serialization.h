#pragma once 

#include "nlohmann/json.hpp"
#include "math.h"

namespace rtr {

class Serialization_manager {
public:
    // 序列化对象到 JSON 字符串
    template<typename T>
    static std::string serialize(const T& obj) {
        nlohmann::json j{};
        to_json(j, obj); 
        return j.dump();
    }

    // 从 JSON 字符串反序列化到对象
    template<typename T>
    static T deserialize(const std::string& json_str) {
        nlohmann::json j = nlohmann::json::parse(json_str);
        T obj{};
        from_json(j, obj); 
        return obj;
    }
};

}

namespace glm {
//glm 序列化
// vec3
inline void to_json(nlohmann::json& j, const glm::vec3& v) {
    j = nlohmann::json::array({v.x, v.y, v.z});
}

inline void from_json(const nlohmann::json& j, glm::vec3& v) {
    v.x = j[0];
    v.y = j[1];
    v.z = j[2];
}

// vec4
inline void to_json(nlohmann::json& j, const glm::vec4& v) {
    j = nlohmann::json::array({v.x, v.y, v.z, v.w});
}
inline void from_json(const nlohmann::json& j, glm::vec4& v) {
    v.x = j[0];
    v.y = j[1];
    v.z = j[2];
    v.w = j[3];
}

// mat4
inline void to_json(nlohmann::json& j, const glm::mat4& m) {
    j = nlohmann::json::array({
        {m[0][0], m[0][1], m[0][2], m[0][3]},
        {m[1][0], m[1][1], m[1][2], m[1][3]},
        {m[2][0], m[2][1], m[2][2], m[2][3]},
        {m[3][0], m[3][1], m[3][2], m[3][3]}
    });
}
inline void from_json(const nlohmann::json& j, glm::mat4& m) {
    m[0][0] = j[0][0];
    m[0][1] = j[0][1];
    m[0][2] = j[0][2];
    m[0][3] = j[0][3];
    m[1][0] = j[1][0];
    m[1][1] = j[1][1];
    m[1][2] = j[1][2];
    m[1][3] = j[1][3];
    m[2][0] = j[2][0];
    m[2][1] = j[2][1];
    m[2][2] = j[2][2];
    m[2][3] = j[2][3];
    m[3][0] = j[3][0];
    m[3][1] = j[3][1];
    m[3][2] = j[3][2];
    m[3][3] = j[3][3];
}

// quat
inline void to_json(nlohmann::json& j, const glm::quat& q) {
    j = nlohmann::json::array({q.x, q.y, q.z, q.w});
}

inline void from_json(const nlohmann::json& j, glm::quat& q) {
    q.x = j[0];
    q.y = j[1];
    q.z = j[2];
    q.w = j[3];
};

};