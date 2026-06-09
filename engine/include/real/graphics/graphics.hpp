#ifndef REALLIB_GRAPHICS_HPP
#define REALLIB_GRAPHICS_HPP

#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"
#include <real/resource/resource_handle.hpp>

namespace real {

typedef void* GraphicsBackend;

class RenderPassCompute;
class RenderPassGeometry;
class RenderPassGeometryInfo;
class ResourceShader;
class ResourceImage;
class RenderPassResource;

struct REALLIB_EXPORT GraphicsInfo {
	bool debug;
	const char *name;
};

/**
 * @brief contains static methods to work with graphics backends to setup static one-instance
 * level thingies.
 */
class REALLIB_EXPORT Graphics {
public:
	/* -------- PLATFORM METHODS -------- */
    static void init_backend(const GraphicsInfo &info);
    static GraphicsBackend get_backend();
    static void destroy_backend();

public:
	/* -------- BUILDER METHODS -------- */
	static Unique<Window> create_window(Instance *instance, const WindowInfo &info);

	static Unique<Renderer> create_renderer(Instance *instance, Shared<Window> window);

	static Unique<RenderPassCompute> create_render_pass_compute(
		Instance *instance, ResourceHandle<ResourceShader> shader,
		std::vector<RenderPassResource> resources);

	static Unique<RenderPassGeometry> create_render_pass_geometry(
		Instance *instance, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> shaders,
		std::vector<RenderPassResource> resources);

	static Unique<ResourceMesh> create_resource_mesh(
		Instance *instance, std::vector<uint32_t> indices, char *data, size_t size);

	static Unique<ResourceImage> create_resource_image(
			Instance *_instance, u32 width, u32 height,
			ColorFormat cformat, ImageFormat iformat, void *data=nullptr, int mips=0);
};

}

#endif
