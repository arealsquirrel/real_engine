#ifndef REALLIB_RENDER_PASS_COMPUTE_HPP
#define REALLIB_RENDER_PASS_COMPUTE_HPP

#include <real/graphics/render_pass.hpp>
#include "real/resource/resource_shader.hpp"
#include <initializer_list>


namespace real {

class RenderPassCompute : public RenderPass {
public:
    RenderPassCompute(
        Instance *_instance,
        ResourceShader* shader);
    ~RenderPassCompute();

    void draw(FrameContext context) override;
    void bind(FrameContext context) override;
};

};

#endif