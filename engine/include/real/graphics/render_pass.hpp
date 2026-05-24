#ifndef REALLIB_RENDER_PASS_HPP
#define REALLIB_RENDER_PASS_HPP

#include "real/core/instance.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <real/graphics/graphics.hpp>

namespace real {

typedef void* RenderPassData;

class Renderer;

/**
 * @brief defines the abstract thingy mabob for actualy rendering shit to the screen
 * 
 */
class RenderPass {
public:
    RenderPass(Instance *_instance,
		std::vector<ResourceHandle<ResourceImage>> _inResources,
		std::vector<ResourceHandle<ResourceImage>> _outResources)
		: instance(_instance), 
		inResources(_inResources), outResources(_outResources) {};

    virtual ~RenderPass() = default;

    virtual void begin_pass(FrameContext context) = 0;
    virtual void end_pass(FrameContext context) = 0;

public:
	const std::vector<ResourceHandle<ResourceImage>> inResources;
	const std::vector<ResourceHandle<ResourceImage>> outResources;
protected:
    Instance *instance;
};

}

#endif
