
#pragma once

#include <real/real.hpp>

using namespace real;

class MyGame : public real::Game {
public:
    using real::Game::Game;
    ~MyGame();

    void start() override;
	void update(u32 delta_time) override;
};

