#ifndef REALLIB_COMPONENT_HPP
#define REALLIB_COMPONENT_HPP

#include "real/core/core.hpp"
#include "real/core/types.hpp"
#include "real/math/mat4.hpp"
#include "real/math/quaternion.hpp"
#include "real/math/vec3.hpp"
#include <real/scene/entity.hpp>

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
    ComponentTransform(Vec3 _position,
                    	Vec3 _rotation,
                        Vec3 _scale)
        : position(_position),
            rotation(_rotation),
            scale(_scale) {}
    ComponentTransform(const ComponentTransform &) = default;

    Mat4 get_transform() const {
		// I actualy cant believe this shit works
		// that is awful
		return math::translate(position)
			* math::make_mat4_from_q(math::make_q_from_euler_angles(rotation.x, rotation.y, rotation.z))
			* math::scale(scale);
    }

    Vec3 position{0.0, 0.0, 0.0};
    Vec3 rotation{0.0, 0.0, 0.0};
    Vec3 scale{1.0, 1.0, 1.0};
};

template <typename... Component>
struct ComponentGroup {
};

};

#endif
