
#pragma once

#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "real/core/game.hpp"
#include "real/graphics/buffer.hpp"
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
    ResourceHandle<ResourceImage> render_texture;

    UniformBuffer *buffer;

    glm::vec4 topGradientColor {1.0f, 0.8f, 0.8f, 1.0f};
    glm::vec4 bottomGradientColor {0.1f, 0.1f, 1.0f, 1.0f};

    glm::mat4 camera_projection;
    glm::mat4 camera_view;

    glm::mat4 model;
};
