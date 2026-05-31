#ifndef REALLIB_RENDER_PASS_COMPUTE_HPP
#define REALLIB_RENDER_PASS_COMPUTE_HPP

#include "real/core/object.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_shader.hpp"
#include <real/graphics/render_pass.hpp>
#include <real/resource/resource_database.hpp>
#include <real/resource/resource_image.hpp>

namespace real {

class REALLIB_EXPORT RenderPassCompute : public RenderPass {
RL_OBJECT(RenderPassCompute, RenderPass)

protected:

	/*
	 * ok so the shader is going to have uniforms and whatnot
	 * that that get read in by the render pass
	 */
    RenderPassCompute(
		Game *_game, ShaderLayout layout,
		std::vector<RenderPassResource> _resources)
		: RenderPass(_game, layout, _resources) {};

public:
    ~RenderPassCompute() = default;

    virtual void begin_pass(FrameContext context) = 0;
    virtual void end_pass(FrameContext context) = 0;
};


};

#endif
