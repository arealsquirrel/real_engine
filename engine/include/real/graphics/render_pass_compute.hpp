#ifndef REALLIB_RENDER_PASS_COMPUTE_HPP
#define REALLIB_RENDER_PASS_COMPUTE_HPP

#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_shader.hpp"
#include <real/graphics/render_pass.hpp>
#include <real/resource/resource_database.hpp>
#include <real/resource/resource_image.hpp>

namespace real {

class RenderPassCompute : public RenderPass {
protected:

	/*
	 * ok so the shader is going to have uniforms and whatnot
	 * that that get read in by the render pass
	 */
    RenderPassCompute(
		Instance *_instance, ShaderLayout layout,
		std::vector<ResourceHandle<ResourceImage>> _inResources,
		std::vector<ResourceHandle<ResourceImage>> _outResources)
		: RenderPass(_instance, layout, _inResources, _outResources) {};

public:
    ~RenderPassCompute() = default;

    void begin_pass(FrameContext context) = 0;
    void end_pass(FrameContext context) = 0;
};


};

#endif
