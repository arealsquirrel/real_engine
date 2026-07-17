#ifndef REALLIB_GRAPHICS_SYSTEM
#define REALLIB_GRAPHICS_SYSTEM

#include "real/core/core.hpp"
#include "real/core/event_listener.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"
#include "real/scene/scene.hpp"
#include "real/scene/system.hpp"

namespace real {

class Instance;
class GraphicsSystem;
class MeshRenderer;
class SpriteRenderer;

struct REALLIB_EXPORT ComponentCamera {
    RL_COMPONENT(ComponentCamera)

    ComponentCamera() = default;

    ~ComponentCamera() = default;
    ComponentCamera(const ComponentCamera &camera) = default;

	Camera camera;
};

class REALLIB_EXPORT GraphicsSystem : public System, public EventListener {
RL_OBJECT(GraphicsSystem, System)

public:
    GraphicsSystem(Instance *_instance, Scene *_scene, Framebuffer *_framebuffer);
    ~GraphicsSystem();

    void awake() override;
    void update(u32 delta_time) override;
    void destroy() override;
	void draw_imgui() override;

	void set_main_camera(EntityHandle entity);
	void bind_main_camera();

private:
	EXPOSE_TO_EDITOR
	Ref<MeshRenderer> mesh_renderer;
	Ref<SpriteRenderer> sprite_renderer;
	EntityHandle camera_entity;

	// TODO : better :/
	Framebuffer *framebuffer;
};

}

#endif
