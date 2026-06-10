
#pragma once

#include "real/core/game.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <real/real.hpp>

using namespace real;

class MyGame : public real::Game {
public:
    using real::Game::Game;
    ~MyGame();

    void start() override;
	void update(u32 delta_time) override;

private:
    RenderPassCompute *compute_pass;
    RenderPassGeometry *geometry_pass;
    ResourceHandle<ResourceMesh> mesh_resource;
    ResourceHandle<ResourceImage> mesh_texture;
};
