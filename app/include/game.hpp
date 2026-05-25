#ifndef APP_GAME_HPP
#define APP_GAME_HPP

#include "real/core/game.hpp"
#include "real/core/types.hpp"
#include "real/graphics/render_pass_compute.hpp"

class AppGame : public real::Game {
public:
	using real::Game::Game;
	~AppGame();

public:
	void start();
	void update(u32 delta_time);

private:
	Shared<real::RenderPassCompute> compute_pass;
};

#endif
