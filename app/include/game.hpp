
#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "real/core/color.hpp"
#include "real/core/game.hpp"
#include "real/core/types.hpp"
#include "real/debug/cvars.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/framebuffer.hpp"
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
    ResourceHandle<ResourceMesh> mesh_resource;
    ResourceHandle<ResourceImage> mesh_texture;

    CVAR_DEF(Color4, topGradientColor, Color4(146, 236, 197));
    CVAR_DEF(Color4, bottomGradientColor, Color4(256,256,256));
};
