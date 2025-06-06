#pragma once

#include "engine/runtime/tool/math.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include <iostream>
#include <memory>

namespace rtr {

class Node : public std::enable_shared_from_this<Node> {

protected:
    glm::mat4 m_model_matrix{glm::identity<glm::mat4>()};
    bool m_is_dirty{true};

    glm::vec3 m_position{glm::zero<glm::vec3>()};
    glm::vec3 m_scale{glm::one<glm::vec3>()};
    glm::quat m_rotation{glm::identity<glm::quat>()};

    std::vector<std::shared_ptr<Node>> m_children{};
    std::weak_ptr<Node> m_parent{};

public:
    Node() {}
  	~Node() {
        if (parent()) {
            parent()->remove_child(shared_from_this());	
        }

        for (auto& child : m_children) {
            child->m_parent.reset();
            child->set_dirty();
        }
    }

	static std::shared_ptr<Node> create() {
		return std::make_shared<Node>();
	}

	const std::shared_ptr<Node> parent() const {
        if (m_parent.expired()) {
            return nullptr;
        }
        return m_parent.lock();
    }

    const std::vector<std::shared_ptr<Node>>& children() const {
        return m_children;
    }

    void add_child(const std::shared_ptr<Node>& node, bool world_position_stays = false) {

        if (node.get() == this) {
            throw std::invalid_argument("Cannot add self as child");
        }
        if (node->parent()) {
            node->parent()->remove_child(node);
        }

        m_children.push_back(node);

        if (world_position_stays) {
            auto world_position = node->world_position();
            auto world_rotation = node->world_rotation();
            auto world_scale = node->world_scale();
            node->m_parent = shared_from_this();
            node->set_world_position(world_position);
            node->set_world_rotation(world_rotation);
            node->set_world_scale(world_scale);

        } else {
            node->m_parent = shared_from_this();
        }

        node->set_dirty();

    }

    void remove_child(const std::shared_ptr<Node>& node) {
        auto it = std::find(m_children.begin(), m_children.end(), node);
        if (it != m_children.end()) {
            m_children.erase(it);
            node->m_parent.reset();
            node->set_dirty();
        } else {
            throw std::invalid_argument("Node is not a child");	
        }
    }

    void set_position(const glm::vec3& pos) {
        m_position = pos;
        set_dirty();
    }

    void set_world_position(const glm::vec3& pos) {
        if (parent()) {
            m_position = pos - parent()->world_position();	
        } else {
            m_position = pos;	
        }
        set_dirty();
    }

    void set_rotation(const glm::quat& rot) {
        m_rotation = rot;
        set_dirty();
    }

    void set_rotation_euler(const glm::vec3& rot) {
        m_rotation = glm::quat(glm::radians(rot));
        set_dirty();	
    }

    void set_world_rotation(const glm::quat& rot) {
        if (parent()) {
            m_rotation = rot * glm::inverse(parent()->world_rotation());	
        }	
        set_dirty();
    }

    void set_scale(const glm::vec3& scale) {
        m_scale = scale;
        set_dirty();
    }

    void set_world_scale(const glm::vec3& scale) {
        if (parent()) {
            m_scale = scale / parent()->world_scale();	
        } else {
            m_scale = scale;
        }
        set_dirty();
    }

    glm::vec3 position() const { return m_position; }
    glm::quat rotation() const { return m_rotation; }
    glm::vec3 scale() const { return m_scale; }
	glm::vec3 rotation_euler() const { return glm::degrees(glm::eulerAngles(m_rotation)); }
	glm::vec3 up() const { return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f); }
    glm::vec3 down() const { return m_rotation * glm::vec3(0.0f, -1.0f, 0.0f); }
    glm::vec3 right() const { return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
	glm::vec3 left() const { return m_rotation * glm::vec3(-1.0f, 0.0f, 0.0f); }
	glm::vec3 front() const { return m_rotation * glm::vec3(0.0f, 0.0f, 1.0f); }
    glm::vec3 back() const { return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f); }

    glm::vec3 world_up() { return world_rotation() * glm::vec3(0.0f, 1.0f, 0.0f); }
    glm::vec3 world_down() { return world_rotation() * glm::vec3(0.0f, -1.0f, 0.0f); }
    glm::vec3 world_right() { return world_rotation() * glm::vec3(1.0f, 0.0f, 0.0f); }
    glm::vec3 world_left() { return world_rotation() * glm::vec3(-1.0f, 0.0f, 0.0f); }
    glm::vec3 world_front() { return world_rotation() * glm::vec3(0.0f, 0.0f, 1.0f); }
    glm::vec3 world_back() { return world_rotation() * glm::vec3(0.0f, 0.0f, -1.0f); }



    glm::mat4 normal_matrix() { return glm::transpose(glm::inverse(model_matrix())); }

    glm::mat4 model_matrix()  {

        if (!m_is_dirty) {
            return m_model_matrix;
        }

        m_is_dirty = false;

        glm::mat4 parent_matrix = glm::identity<glm::mat4>();

        if (parent()) {
            parent_matrix = parent()->model_matrix();
        }

        glm::mat4 transform = glm::identity<glm::mat4>();

        transform = glm::scale(transform, m_scale);
        transform *= glm::mat4_cast(m_rotation);
        transform = glm::translate(glm::identity<glm::mat4>(), m_position) * transform;
        
        return m_model_matrix = parent_matrix * transform;
    }

    void set_local_model_matrix(const glm::mat4& local_model_matrix) {
        glm::vec3 scale = glm::vec3(
            glm::length(glm::vec3(local_model_matrix[0])),
            glm::length(glm::vec3(local_model_matrix[1])),
            glm::length(glm::vec3(local_model_matrix[2]))
        );

        glm::mat3 rotation_matrix = glm::mat3(local_model_matrix);
        rotation_matrix[0] /= scale.x;
        rotation_matrix[1] /= scale.y;
        rotation_matrix[2] /= scale.z;
        glm::quat rotation = glm::quat_cast(rotation_matrix);
        glm::vec3 position = glm::vec3(local_model_matrix[3]);
        
        set_position(position);
        set_rotation(rotation);
        set_scale(scale);

        m_is_dirty = true;
    }

    void set_dirty() { 
        if (m_is_dirty) {
            return;	
        }

        m_is_dirty = true; 
        for (auto& child : m_children) {
            child->set_dirty();	
        }
    }

	glm::vec3 world_scale() {
        return glm::vec3(
			glm::length(glm::vec3(model_matrix()[0])),
			 glm::length(glm::vec3(model_matrix()[1])), 
			 glm::length(glm::vec3(model_matrix()[2]))
			);
    }

    glm::vec3 world_position() {
        return glm::vec3(model_matrix()[3]);
    }

    glm::quat world_rotation() {
		auto scale = world_scale();
		auto model_matrix = glm::mat3(this->model_matrix());
		model_matrix[0] /= scale.x;
		model_matrix[1] /= scale.y;
		model_matrix[2] /= scale.z;
        return glm::quat_cast(model_matrix);
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

        set_rotation(glm::normalize(rotation_quat * m_rotation)); 
    }

    void look_at_point(const glm::vec3& target_point) {
        glm::vec3 direction = target_point - position();
        look_at_direction(direction);
    }

    void translate(const glm::vec3 &direction, float distance) {
        set_position(m_position + direction * distance);
    }

    void rotate(float angle, const glm::vec3& axis) {
        glm::quat rotation = glm::angleAxis(glm::radians(angle), axis);  // Convert angle-axis to quaternion
        set_rotation(rotation * m_rotation);  // Apply rotation to current rotation
    }
	
};

};


