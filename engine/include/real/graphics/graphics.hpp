#ifndef REALLIB_GRAPHICS_HPP
#define REALLIB_GRAPHICS_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include <real/resource/resource_handle.hpp>

namespace real {

typedef void* GraphicsBackend;

class RenderPassCompute;
class ResourceShader;
class ResourceImage;

struct GraphicsInfo {
	bool debug;
	const char *name;
};

/**
 * @brief contains static methods to work with graphics backends to setup static one-instance
 * level thingies.
 */
class Graphics {
public:
	/* -------- PLATFORM METHODS -------- */
    static void init_backend(const GraphicsInfo &info);
    static GraphicsBackend get_backend();
    static void destroy_backend();

public:
	/* -------- BUILDER METHODS -------- */
	static Shared<Window> create_window(Instance *instance, const WindowInfo &info);

	static Shared<Renderer> create_renderer(Instance *instance, Shared<Window> window);

	static Shared<RenderPassCompute> create_render_pass_compute(
		Instance *_instance, ResourceHandle<ResourceShader> shader,
		std::vector<ResourceHandle<ResourceImage>> _inResources,
		std::vector<ResourceHandle<ResourceImage>> _outResources);
};

}

#endif
