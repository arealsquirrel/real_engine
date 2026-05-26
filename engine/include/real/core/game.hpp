#ifndef REALLIB_GAME_HPP
#define REALLIB_GAME_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"

namespace real {

class Game {
public:
	Game(Shared<Instance> _instance);
	~Game();

public:
	virtual void start() = 0;
	virtual void update(u32 delta_time) = 0;
	virtual void render(real::FrameContext frame) = 0;

protected:
	Shared<Instance> instance;
};

}

#endif
