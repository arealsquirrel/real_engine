#ifndef REALLIB_RENDERER_HPP
#define REALLIB_RENDERER_HPP

#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include <real/core/core.hpp>

namespace real {

class ResourceImage;

// class Instance;

/**
 * @brief this class holds the render passes as well as the command buffers and data
 * for rendering crap. It also has to manage the textures for rendering and whatnot.
 *
 * its attached to a window
 */
class REALLIB_EXPORT Renderer : public Object {
RL_OBJECT(Renderer, Object)

public:
    Renderer(Instance *_instance, Shared<Window> _window);
    virtual ~Renderer();

	// some of the constructors need instance things to be initilized
	virtual void init() = 0;

    virtual void start_frame() = 0;
    virtual void end_frame() = 0;
    virtual void copy_image_to_screen(ResourceHandle<ResourceImage> image) = 0;

protected:
    Shared<Window> window;
};

}

#endif
