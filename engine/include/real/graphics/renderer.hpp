#ifndef REALLIB_RENDERER_HPP
#define REALLIB_RENDERER_HPP

#include "real/core/types.hpp"
#include "real/graphics/window.hpp"

namespace real {

typedef void* RendererData;
typedef void* FrameContext;

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

    FrameContext start_frame();
    void end_frame(FrameContext context);

    RendererData get_handle() { return render_data; };

private:
    RendererData render_data;
    Instance *instance;
    Window *window;

    u32 frame_number=0;
};

}

#endif