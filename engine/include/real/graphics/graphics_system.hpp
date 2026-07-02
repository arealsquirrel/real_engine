#ifndef REALLIB_GRAPHICS_SYSTEM
#define REALLIB_GRAPHICS_SYSTEM

#include "real/core/color.hpp"
#include "real/core/core.hpp"
#include "real/core/event_listener.hpp"
#include "real/core/object.hpp"
#include "real/core/string_hash.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"
#include "real/scene/system.hpp"

namespace real {

class Instance;

struct REALLIB_EXPORT ComponentMeshRenderer {
    RL_COMPONENT(ComponentMeshRenderer)

    ComponentMeshRenderer(
			ResourceHandle<ResourceMesh> _mesh,
			ResourceHandle<ResourceImage> _texture)
        : mesh(_mesh), texture(_texture), sub_mesh(_mesh.get()->meshes.begin()->second) {}

    ComponentMeshRenderer(
			ResourceHandle<ResourceMesh> _mesh,
			ResourceHandle<ResourceImage> _texture, StringHash hash)
        : mesh(_mesh), texture(_texture), sub_mesh(_mesh.get()->meshes.at(hash)) {}

    ComponentMeshRenderer(
			ResourceHandle<ResourceMesh> _mesh,
			ResourceHandle<ResourceImage> _texture, ResourceMesh::Mesh _sub_mesh)
        : mesh(_mesh), texture(_texture), sub_mesh(_sub_mesh) {}

    ComponentMeshRenderer(const ComponentMeshRenderer &) = default;
    ~ComponentMeshRenderer() = default;

    ResourceHandle<ResourceMesh> mesh;
    ResourceHandle<ResourceImage> texture;
	ResourceMesh::Mesh sub_mesh;
};

struct REALLIB_EXPORT ComponentCamera {
    RL_COMPONENT(ComponentCamera)

    ComponentCamera() = default;
    ~ComponentCamera() = default;
    ComponentCamera(const ComponentCamera &camera) = default;

    Camera camera;
    Color4 clear_color {0.0f, 0.0f, 0.0f, 1.0f};
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

    Framebuffer *get_framebuffer() { return framebuffer; };
    void set_framebuffer(Framebuffer *_framebuffer) { framebuffer = _framebuffer; }
    void set_main_camera(EntityHandle handle) { main_camera = handle; }

private:
    RenderPassGeometry *diffuse_pass;
    Framebuffer *framebuffer;
    UniformBuffer *camera_uniform_buffer;
    
    EntityHandle main_camera;
};

}

#endif
