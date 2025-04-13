#pragma once 

#include "nlohmann/json.hpp"

namespace rtr {

class Serialization_manager {
public:
    // 序列化对象到 JSON 字符串
    template<typename T>
    std::string serialize(const T& obj) {
        nlohmann::json j{};
        to_json(j, obj); 
        return j.dump();
    }

    // 从 JSON 字符串反序列化到对象
    template<typename T>
    T deserialize(const std::string& jsonStr) {
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        T obj{};
        from_json(j, obj); 
        return obj;
    }
};

// 示例：自定义类型的 to_json 和 from_json 函数
struct ExampleType {
    int value;
};

// 自定义类型的序列化函数
inline void to_json(nlohmann::json& j, const ExampleType& p) {
    j = nlohmann::json{{"value", p.value}};
}

// 自定义类型的反序列化函数
inline void from_json(const nlohmann::json& j, ExampleType& p) {
    j.at("value").get_to(p.value);
}

}