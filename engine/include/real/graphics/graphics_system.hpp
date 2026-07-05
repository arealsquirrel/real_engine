#ifndef REALLIB_GRAPHICS_SYSTEM
#define REALLIB_GRAPHICS_SYSTEM

#include "real/core/color.hpp"
#include "real/core/core.hpp"
#include "real/core/event_listener.hpp"
#include "real/core/object.hpp"
#include "real/core/string_hash.hpp"
#include "real/core/types.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"
#include "real/scene/scene.hpp"
#include "real/scene/system.hpp"
#include <memory>
#include <type_traits>
#include <variant>
#include <vector>

namespace real {

class Instance;
class GraphicsSystem;

struct REALLIB_EXPORT ComponentMeshRenderer {
    RL_COMPONENT(ComponentMeshRenderer)

    ComponentMeshRenderer(
			ResourceHandle<ResourceMesh> _mesh,
			ResourceHandle<ResourceImage> _texture)
        : mesh(_mesh), texture(_texture),
		  sub_mesh(_mesh.get()->meshes.begin()->second),
		  tile(_texture.get()->tiles[StringHash("_full_image")]){}

    ComponentMeshRenderer(
			ResourceHandle<ResourceMesh> _mesh,
			ResourceHandle<ResourceImage> _texture, 
			StringHash submesh_name, StringHash tile_name=StringHash("_full_image"))
        : mesh(_mesh), texture(_texture),
		  sub_mesh(_mesh.get()->meshes.at(submesh_name)),
		  tile(_texture.get()->tiles.at(tile_name)) {}

    ComponentMeshRenderer(const ComponentMeshRenderer &) = default;
    ~ComponentMeshRenderer() = default;

    ResourceHandle<ResourceMesh> mesh;
    ResourceHandle<ResourceImage> texture;
	ResourceMesh::Mesh sub_mesh;
	ResourceImage::Tile tile;
};

struct REALLIB_EXPORT ComponentCamera {
    RL_COMPONENT(ComponentCamera)

    ComponentCamera() = default;
    ~ComponentCamera() = default;
    ComponentCamera(const ComponentCamera &camera) = default;

    Camera camera;
    Color4 clear_color {0.0f, 0.0f, 0.0f, 1.0f};
};

/**
 * renders something somewere <3
 */
class REALLIB_EXPORT SubRenderer : public Object {
RL_OBJECT(SubRenderer, Object)
	SubRenderer(Instance *_instance, GraphicsSystem *_graphics_system, Scene *_scene);
	~SubRenderer() = default;

public:
	virtual void awake() = 0;
	virtual void render(u32 deltatime) = 0;
	virtual void destroy() = 0;

protected:
	GraphicsSystem *graphics_system;
	Scene *scene;
};

/**
 * Renders a post processing effect
 */
class REALLIB_EXPORT PostEffect : public Object {
RL_OBJECT(PostEffect, Object)

public:
	PostEffect(Instance *_instance, GraphicsSystem *_graphics_system, Scene *_scene);
	~PostEffect() = default;

	virtual void awake() = 0;
	virtual void render(u32 deltatime) = 0;
	virtual void destroy() = 0;

protected:
	GraphicsSystem *graphics_system;
	Scene *scene;
};

class REALLIB_EXPORT GraphicsSystem : public System, public EventListener {
RL_OBJECT(GraphicsSystem, System)

public:
    GraphicsSystem(Instance *_instance, Scene *_scene, Framebuffer *_framebuffer);
    ~GraphicsSystem();

	/**
	 * if you call this after everything has been awakened, the game will freaking crash
	 */
	template<typename T, typename ...Args>
	Shared<T> add_post_effect(Args &&...args) {
		static_assert(std::is_base_of_v<PostEffect, T>, "must derive from post effect");
		auto ptr = std::make_shared<T>(instance, this, scene, std::forward<Args>(args)...);
		post_effects.push_back(ptr);
		return ptr;
	}

	template<typename T, typename ...Args>
	Shared<T> add_subrenderer(Args &&...args) {
		static_assert(std::is_base_of_v<SubRenderer, T>, "must derive from SubRenderer");
		auto ptr = std::make_shared<T>(instance, this, scene, std::forward<Args>(args)...);
		sub_renderers.push_back(ptr);
		return ptr;
	}

    void awake() override;
    void update(u32 delta_time) override;
    void destroy() override;
	void draw_imgui() override;

    Framebuffer *get_framebuffer() { return framebuffer; };
    void set_framebuffer(Framebuffer *_framebuffer) { framebuffer = _framebuffer; }
    void set_main_camera(EntityHandle handle) { main_camera = handle; }
    void set_main_camera(Shared<Camera> camera) { main_camera = camera; }
	BufferHandle get_camera_uniform_buffer_handle() { return camera_uniform_buffer->get_handle(); };

private:
	EXPOSE_TO_EDITOR

	std::vector<Shared<PostEffect>> post_effects;
	std::vector<Shared<SubRenderer>> sub_renderers;

    Framebuffer *framebuffer;
    Unique<UniformBuffer> camera_uniform_buffer;

	std::variant<EntityHandle, Shared<Camera>> main_camera;
};

class SubRendererDiffuse3D : public SubRenderer {
RL_OBJECT(SubRendererDiffuse3D, SubRenderer)

public:
	using SubRenderer::SubRenderer;
	~SubRendererDiffuse3D() = default;

	void awake() override;
	void render(u32 deltatime) override;
	void destroy() override;

private:
	Unique<RenderPassGeometry> diffuse_pass;
};

}

#endif
