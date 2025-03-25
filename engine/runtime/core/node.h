#pragma once
#include "engine/global/base.h"
#include "engine/global/guid.h"

#include "engine/runtime/enum.h"

#include "shader.h"

namespace rtr {

class Node : public std::enable_shared_from_this<Node>, public GUID {
protected:
    Node_type m_type{};

    glm::vec3 m_position = glm::zero<glm::vec3>();
    glm::vec3 m_scale = glm::one<glm::vec3>();
    glm::quat m_rotation = glm::identity<glm::quat>();

    std::vector<std::shared_ptr<Node>> m_children{};
    std::weak_ptr<Node> m_parent{};

public:
    explicit Node(Node_type type = Node_type::NODE) : GUID(), m_type(type) {}
    virtual ~Node() = default;

    std::shared_ptr<Node> parent_ptr() {
        if (m_parent.expired()) {
            return nullptr;
        }
        return m_parent.lock();
    }

    std::vector<std::shared_ptr<Node>>& children() {
        return m_children;
    }

    void add_child(const std::shared_ptr<Node>& node) {
        m_children.push_back(node);
        node->m_parent = shared_from_this();
    }

    void erase_child(const std::shared_ptr<Node> &node) {
        auto it = std::find(m_children.begin(), m_children.end(), node);
        if (it != m_children.end()) {
            m_children.erase(it);
            node->m_parent.reset();
        }
    }

    void clear_children() {
        for (auto& child : m_children) {
            child->m_parent.reset();
        }

        m_children.clear();
    }

    [[nodiscard]] Node_type type() const { 
        return m_type; 
    }

    [[nodiscard]] glm::vec3 position() const {
        return m_position;
    }

    [[nodiscard]] glm::quat rotation() const {
        return m_rotation;
    }

    [[nodiscard]] glm::vec3 rotation_euler() const {
        return glm::degrees(glm::eulerAngles(m_rotation));
    }

    [[nodiscard]] glm::vec3 scale() const {
        return m_scale;
    }

    [[nodiscard]] glm::vec3 up() const {
        return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    [[nodiscard]] glm::vec3 down() const {
        return m_rotation * glm::vec3(0.0f, -1.0f, 0.0f);
    }

    [[nodiscard]] glm::vec3 right() const {
        return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    [[nodiscard]] glm::vec3 left() const {
        return m_rotation * glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    [[nodiscard]] glm::vec3 front() const {
        return m_rotation * glm::vec3(0.0f, 0.0f, 1.0f);
    }

    [[nodiscard]] glm::vec3 back() const {
        return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::quat& rotation() {
        return m_rotation;
    }

    glm::vec3& scale() {
        return m_scale;
    }

    glm::vec3& position() {
        return m_position;
    }

    [[nodiscard]] glm::mat4 normal_matrix() {
        return glm::transpose(glm::inverse(model_matrix()));
    }

    [[nodiscard]] glm::mat4 model_matrix() {

        glm::mat4 parent_matrix = glm::identity<glm::mat4>();

        if (parent_ptr()) {
            parent_matrix = parent_ptr()->model_matrix();
        }

        glm::mat4 transform = glm::identity<glm::mat4>();

        transform = glm::scale(transform, m_scale);
        transform *= glm::mat4_cast(m_rotation);
        transform = glm::translate(glm::identity<glm::mat4>(), m_position) * transform;

        return parent_matrix * transform;

    }

    Node world_node() {
        Node node(type());

        node.m_position = position();
        node.m_rotation = rotation();
        node.m_scale = scale();

        if (parent_ptr()) {
            node.m_position = parent_ptr()->world_node().position() + node.m_position;
            node.m_rotation = parent_ptr()->world_node().rotation() * node.m_rotation;
            node.m_scale = parent_ptr()->world_node().scale() * node.m_scale;
        }

        return node;
    }

    void look_at_direction(const glm::vec3& target_direction) {
        if (target_direction == glm::zero<glm::vec3>()) {
            std::cout << "target_direction is zero" << std::endl;
            return;
        }

        glm::vec3 direction = glm::normalize(target_direction);
        
        if (glm::length(direction) < EPSILON) {
            return;
        }

        glm::vec3 current_front = front();

        // 检查当前前向量和目标方向是否几乎平行
        if (glm::length(glm::cross(current_front, direction)) < EPSILON) {
            // 如果它们是相反方向，则绕 up 轴旋转 180 度
            if (glm::dot(current_front, direction) < 0) {
                m_rotation = glm::rotate(m_rotation, glm::radians(180.0f), up());
            }
            return;
        }

        // 计算旋转四元数：从 current_front 旋转到 direction
        glm::quat rotation_quat = glm::rotation(current_front, direction);

        // 更新 m_rotation
        m_rotation = rotation_quat * m_rotation;

    }

    void look_at_point(const glm::vec3& target_point) {
        glm::vec3 direction = target_point - position();
        look_at_direction(direction);
    }

    void translate(const glm::vec3 &direction, float distance) {
        m_position += direction * distance;
    }

    void rotate(float angle, const glm::vec3& axis) {
        m_rotation = glm::rotate(m_rotation, glm::radians(angle), axis);
    }

};

};