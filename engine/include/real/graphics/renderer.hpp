#ifndef REALLIB_RENDERER_HPP
#define REALLIB_RENDERER_HPP

#include <real/core/event.hpp>
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include <real/core/core.hpp>

namespace real {

class ResourceImage;

struct RendererStats {
    Timer frame_time;
    u32 verticies;
    u32 indicies;
    u32 draw_calls;
};

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
    virtual void end_frame(const ResourceImage *copy_to_screen_image=nullptr) = 0;

	RendererStats render_stats;

	static Unique<Renderer> create(Instance *instance, Shared<Window> window);

protected:
    Shared<Window> window;
};

}

#endif
