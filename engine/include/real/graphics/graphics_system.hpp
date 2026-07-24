#ifndef REALLIB_GRAPHICS_SYSTEM
#define REALLIB_GRAPHICS_SYSTEM

#include "real/container/color.hpp"
#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include "real/core/event_listener.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/buffer.hpp"
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

struct SceneData {
    Mat4 proj;
    Mat4 view;
	Vec3 view_position;
	float ambient_power;
	Vec4 light_color;
	Vec4 light_position;
};

struct REALLIB_EXPORT ComponentCamera {
    RL_COMPONENT(ComponentCamera)

    ComponentCamera()
		: clear_color(0,0,0,1) {}

    ~ComponentCamera() = default;
    ComponentCamera(const ComponentCamera &camera) = default;

	Camera camera;
	Color4 clear_color; 
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

public:
	/**
	 * the renderers will push the draw commands to the buffers, but will not call
	 * the draw function.
	 */
	bool draw_commands {true};


	/**
	 * the renderers will bind the camera on the main camera entity
	 * at the start of each frame.
	 */
	bool bind_main_camera {true};

	EntityHandle main_camera_entity;

	UniquePointer<UniformBuffer> scene_data;

private:
	EXPOSE_TO_EDITOR
	Ref<MeshRenderer> mesh_renderer;
	Ref<SpriteRenderer> sprite_renderer;

	Framebuffer *framebuffer;
};

}

#endif
