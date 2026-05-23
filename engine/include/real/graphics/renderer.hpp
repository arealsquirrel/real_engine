#ifndef REALLIB_RENDERER_HPP
#define REALLIB_RENDERER_HPP

#include "real/core/types.hpp"
#include "real/graphics/window.hpp"

namespace real {

// typedef void* RendererData;
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
    Renderer(Instance *_instance, Shared<Window> _window);
    virtual ~Renderer();

	// some of the constructors need instance things to be initilized
	virtual void init() = 0;

    virtual FrameContext start_frame() = 0;
    virtual void end_frame(FrameContext context) = 0;

protected:
    Instance *instance;
    Shared<Window> window;
};

}

#endif
