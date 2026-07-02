
#pragma once

#include <real/real.hpp>

using namespace real;

class MyGame : public real::Game {
public:
    using real::Game::Game;
    ~MyGame();

    void start() override;
	void update(u32 delta_time) override;

private:
    ResourceHandle<ResourceImage> mesh_texture;

    CVAR_DEF(Color4, topGradientColor, Color4(146, 236, 197));
    CVAR_DEF(Color4, bottomGradientColor, Color4(256,256,256));
};
