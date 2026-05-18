#ifndef REALLIB_RENDER_PASS_HPP
#define REALLIB_RENDER_PASS_HPP

#include "real/core/instance.hpp"
#include "real/graphics/window.hpp"
#include <real/graphics/graphics.hpp>
#include <string>

namespace real {

typedef void* RenderPassData;

class Renderer;

/**
 * @brief defines the abstract thingy mabob for actualy rendering shit to the screen
 * 
 */
class RenderPass {
public:
    RenderPass(Instance *_instance)
        : instance(_instance) {}

    virtual ~RenderPass() = default;

    virtual void bind(FrameContext context) = 0;
    virtual void draw(FrameContext context) = 0;

protected:
    RenderPassData data;
    Instance *instance;
};

}

#endif