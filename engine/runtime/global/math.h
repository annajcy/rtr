#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/fwd.hpp"

#include <cmath>
#include <limits>
#include <vector>

namespace rtr {

constexpr float PI = 3.14159265358979323846f;
constexpr float EPSILON = 1e-6f;

struct Ray {
    glm::vec3 origin{};
    glm::vec3 direction{};

    Ray() : origin(glm::vec3(0.0f)), direction(glm::vec3(0.0f)) {}
    Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {}
    glm::vec3 at(float t) const {
        return origin + direction * t;
    }

};


struct Bouding_box {
    glm::vec3 min{};
    glm::vec3 max{};

    Bouding_box() : 
    min(glm::vec3(std::numeric_limits<float>::max())), 
    max(glm::vec3(std::numeric_limits<float>::min())) {}

    Bouding_box(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    Bouding_box(const std::vector<glm::vec3>& points) {
        for (const auto& point : points) {
            min = glm::min(min, point);
            max = glm::max(max, point);
        }
    }

    Bouding_box operator+(const glm::vec3& point) const {
        auto min_ = glm::min(min, point);
        auto max_ = glm::max(max, point);
        return Bouding_box{min_, max_};
    }

    Bouding_box& operator+=(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
        return *this;
    }

    Bouding_box operator+(const Bouding_box& other) const {
        auto min_ = glm::min(min, other.min);
        auto max_ = glm::max(max, other.max);
        return Bouding_box{min_, max_};
    }

    Bouding_box& operator+=(const Bouding_box& other) {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
        return *this;
    }

    Bouding_box operator*(const glm::mat4& transform) const {
        auto min_ = glm::vec3(transform * glm::vec4(min, 1.0f));
        auto max_ = glm::vec3(transform * glm::vec4(max, 1.0f));
        return Bouding_box{min_, max_};
    }

    Bouding_box& operator*=(const glm::mat4& transform) {
        auto min_ = glm::vec3(transform * glm::vec4(min, 1.0f));
        auto max_ = glm::vec3(transform * glm::vec4(max, 1.0f));
        min = min_;
        max = max_;
        return *this;
    }

    bool overlap(const Bouding_box& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }

    glm::vec3 center() const {
        return (min + max) * 0.5f;
    }

    glm::vec3 extent() const {
        return max - min;
    }

};

struct Sphere {
    glm::vec3 center{};
    float radius{};
    Sphere() : center(glm::vec3(0.0f)), radius(0.0f) {}
    Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}
    Sphere(const Bouding_box& bbox) : center(bbox.center()), radius(glm::length(bbox.extent()) * 0.5f) {}

};


struct Triangle {
    glm::vec3 v0{};
    glm::vec3 v1{};
    glm::vec3 v2{};
    Triangle() : v0(glm::vec3(0.0f)), v1(glm::vec3(0.0f)), v2(glm::vec3(0.0f)) {}
    Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) : v0(v0), v1(v1), v2(v2) {}
    
    glm::vec3 normal() const {
        return glm::normalize(glm::cross(v1 - v0, v2 - v0));
    }

    glm::vec3 centroid() const {
        return (v0 + v1 + v2) / 3.0f;
    }

    float area() const {
        return 0.5f * glm::length(glm::cross(v1 - v0, v2 - v0));
    }

};

struct Plane {
    glm::vec3 normal{};
    float d{};
    Plane() : normal(glm::vec3(0.0f)), d(0.0f) {}
    Plane(const glm::vec3& normal, float d) : normal(normal), d(d) {}
    Plane(const Triangle& triangle) : normal(triangle.normal()), d(glm::dot(normal, triangle.v0)) {}
    Plane(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) : normal(glm::normalize(glm::cross(v1 - v0, v2 - v0))), d(glm::dot(normal, v0)) {}
    float distance(const glm::vec3& point) const {
        return glm::dot(normal, point) - d;
    }
     
};



};