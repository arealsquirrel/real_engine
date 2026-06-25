#ifndef REALLIB_COMPONENT_HPP
#define REALLIB_COMPONENT_HPP

#include "glm/ext/matrix_float4x4.hpp"
#include "real/core/core.hpp"
#include "real/core/types.hpp"
#include <real/scene/entity.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace real {

#define RL_COMPONENT(name) static const char *get_name() { return #name; }

struct REALLIB_EXPORT ComponentID {
    RL_COMPONENT(ComponentID)

    ComponentID(String _name, UUID _id)
        : name(_name), id(_id) {}
    ~ComponentID() = default;
    ComponentID(const ComponentID &) = default;

    String name;
    UUID id;
};

struct REALLIB_EXPORT ComponentTransform {
    RL_COMPONENT(ComponentTransform)

    ComponentTransform() = default;
    ComponentTransform(glm::vec3 _position,
                        glm::vec3 _rotation,
                        glm::vec3 _scale)
        : position(_position),
            rotation(_rotation),
            scale(_scale) {}
    ComponentTransform(const ComponentTransform &) = default;

    glm::mat4 get_transform() const {
        glm::mat4 rot = glm::toMat4(glm::quat(rotation));

        return glm::translate(glm::mat4(1.0f), position)
            * rot
            * glm::scale(glm::mat4(1.0f), scale);
    }

    glm::vec3 position{0.0, 0.0, 0.0};
    glm::vec3 rotation{0.0, 0.0, 0.0};
    glm::vec3 scale{1.0, 1.0, 1.0};
};

template <typename... Component>
struct ComponentGroup {
};

};

#endif
