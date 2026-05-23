#ifndef REALLIB_RENDER_PASS_COMPUTE_HPP
#define REALLIB_RENDER_PASS_COMPUTE_HPP

#include <real/graphics/render_pass.hpp>
#include "real/resource/resource_shader.hpp"
#include <real/resource/resource_database.hpp>
#include <real/resource/resource_image.hpp>

namespace real {

class RenderPassCompute : public RenderPass {
public:

	/*
	 * ok so the shader is going to have uniforms and whatnot
	 * that that get read in by the render pass
	 */
    RenderPassCompute(
		Instance *_instance,
        ResourceShader* shader,
		std::vector<ResourceHandle<ResourceImage>> inResources,
		std::vector<ResourceHandle<ResourceImage>> outResources);

    ~RenderPassCompute();

    void begin_pass(FrameContext context) override;
    void end_pass(FrameContext context) override;
};


};

#endif
