#ifndef REALLIB_SYSTEM_HPP
#define REALLIB_SYSTEM_HPP

#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"

namespace real {

class Scene;

/**
 * @brief defines a system that performs operations on entities.
 */
class REALLIB_EXPORT System : public Object {
RL_OBJECT(System, Object)

public:
    /**
     * @brief Construct a new System object. all systems must have this same constructor.
     * 
     * @param _instance the attached instance
     * @param _scene the attached scene
     */
    System(Instance *_instance, Scene *_scene);

    /**
     * @brief Construct a new System object from anther system. You need to do a deep copy
     * 
     * @param system
     */
    System(const System &system);

    virtual ~System();

    /**
     * @brief called once at the begining of the scene.
     */
    virtual void awake() = 0;

    /**
     * @brief called every frame, iterate over components and do stuff
     */
    virtual void update(u32 delta_time) = 0;

    /**
     * @brief called once at the end of the scene when everything shuts down.
     */
    virtual void destroy() = 0;

	virtual void draw_imgui();

protected:
    Scene *scene;
};

}

#endif
