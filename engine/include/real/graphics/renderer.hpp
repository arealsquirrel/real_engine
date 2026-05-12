#ifndef REALLIB_RENDERER_HPP
#define REALLIB_RENDERER_HPP

#include "real/core/types.hpp"
#include "real/graphics/window.hpp"

namespace real {

typedef void* RendererData;

class Instance;

/**
 * @brief this class holds the render passes as well as the command buffers and data
 * for rendering crap. It also has to manage the textures for rendering and whatnot.
 *
 * its attached to a window
 */
class Renderer {
public:
    Renderer(Instance *_instance, Window *_window);
    ~Renderer();

    /**
     * @brief the magic command that draws the things to the screen
     *
     */
    void draw();

private:
    RendererData render_data;
    Instance *instance;
    Window *window;

    u32 frame_number=0;
};

}

#endif