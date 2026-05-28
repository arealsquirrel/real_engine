#ifndef APP_GAME_HPP
#define APP_GAME_HPP

#include "real/core/game.hpp"
#include "real/core/types.hpp"
#include "real/graphics/render_pass_compute.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/graphics/renderer.hpp"

class AppGame : public real::Game {
public:
	using real::Game::Game;
	~AppGame();

public:
	void start();
	void update(u32 delta_time);
	void render(real::FrameContext frame);

private:
	Shared<real::RenderPassCompute> compute_pass;
	Shared<real::RenderPassGeometry> geometry_pass;
};

#endif
