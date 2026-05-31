#ifndef REALLIB_GAME_HPP
#define REALLIB_GAME_HPP

#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_database.hpp"
#include <real/core/core.hpp>

namespace real {

class REALLIB_EXPORT Game {
public:
	Game();
	~Game();

public:
	void start();
	void update(u32 delta_time);
	void render(real::FrameContext frame);
	void destroy();

public:
	bool should_close();

public:
	Shared<ResourceDatabase> resource_database;
	Shared<Window> window;
    Shared<Renderer> renderer;
};

}

#endif
