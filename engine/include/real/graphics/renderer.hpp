#ifndef REALLIB_RENDERER_HPP
#define REALLIB_RENDERER_HPP

#include <real/core/event.hpp>
#include "real/core/object.hpp"
#include "real/core/object_container.hpp"
#include "real/core/types.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/window.hpp"
#include <real/core/core.hpp>

namespace real {

class ResourceImage;
class Framebuffer;

struct RendererStats {
    Timer frame_time;
    u32 verticies;
    u32 indicies;
    u32 draw_calls;
};

struct SceneData {
    alignas(16) Mat4 proj;
    alignas(16) Mat4 view;
};

/**
 * renders something somewere <3
 */
class REALLIB_EXPORT SubRenderer : public Object {
RL_OBJECT(SubRenderer, Object)
	SubRenderer(Instance *_instance, Renderer *_renderer);
	~SubRenderer() = default;

public:
	virtual void draw_commands(Framebuffer *framebuffer) = 0;

	/* flushes the commands */
	virtual void flush_commands() = 0;

	virtual void destroy() = 0;

protected:
	Renderer *renderer;
};

/**
 *
 * Renders a post processing effect
 */
class REALLIB_EXPORT PostEffect : public Object {
RL_OBJECT(PostEffect, Object)

public:
	PostEffect(Instance *_instance, Renderer *_renderer);
	~PostEffect() = default;

	/* draws the commands, but does not flush them! */
	virtual void draw_commands() = 0;

	/* flushes the commands */
	virtual void flush_commands() = 0;

	virtual void destroy() = 0;

protected:
	Renderer *renderer;
};

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

	/**
	 * sets up the graphics API to draw a frame
	 */
    virtual void start_frame() = 0;

	/**
	 * ends the frame and uploads the commands to the GPU
	 */
    virtual void end_frame(const ResourceImage *copy_to_screen_image=nullptr) = 0;

	/* 
	 * calls the draw function for all subrenderers and
	 * uploads their commands to the API cmd buffer to be sent to the GPU
	 */
	void draw_render_frame();

	/* calls the draw function for all post effects */
	void draw_post_frame();

	/* flushes the commands for the renderer */
	void flush_all();

	/* 
	 * uploads the camera's data to the uniform buffer
	 * scene data for use by the subrenderers
	 */
	void attach_camera(Camera &camera);

	/**
	 * must be called before the object is destructed or the program
	 * segfaults at close
	 */
	void destroy_renderers();


	static Unique<Renderer> create(Instance *instance, Shared<Window> window);

public:
	
	// the graphics API must take care of destroying these objects
	UniqueObjectSet<SubRenderer> subrenderers;
	UniqueObjectSet<PostEffect> post_effects;

	RendererStats render_stats;

	// created by the graphics API
	Unique<UniformBuffer> scene_data;

protected:
    Shared<Window> window;
};

}

#endif
