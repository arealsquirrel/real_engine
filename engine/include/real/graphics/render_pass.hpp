#ifndef REALLIB_RENDER_PASS_HPP
#define REALLIB_RENDER_PASS_HPP

#include <real/graphics/graphics.hpp>

namespace real {

/**
 * @brief defines the abstract thingy mabob for actualy rendering shit to the screen
 * 
 */
class RenderPass {
public:
    RenderPass() = default;
    virtual ~RenderPass();

    virtual void draw(Renderer *attached_renderer) = 0;
};

}

#endif