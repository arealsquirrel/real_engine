
#include "game.hpp"

#include <memory>
#include <real/real.hpp>
#include <imgui.h>
#include <glm/glm.hpp>
#include "glm/ext/vector_float3.hpp"
#include "real/core/color.hpp"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/string_hash.hpp"
#include "real/core/types.hpp"
#include "real/debug/cvars.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass.hpp"
#include "real/graphics/renderpass_compute.hpp"
#include "real/resource/resource_shader.hpp"

using namespace real;

EXPOSE_GAME_TO_REAL(MyGame)

class MyPostEffect : public real::PostEffect {
public:
	using real::PostEffect::PostEffect;
	~MyPostEffect() = default;

	void awake() {
		auto shader = instance->resource_database->load_resource_disk<ResourceShader>(
				"../engine/resources/shaders/gradient.slang.spv");

		RenderPassResource renderpass_resource;
		renderpass_resource.texture = graphics_system->get_framebuffer()->get_color_resolve_image();

		compute = RenderPassCompute::create(instance, shader, {renderpass_resource});
	}

	void render(u32 deltatime) {
		compute->begin_pass();
		compute->set_variable("topColor", topGradientColor);
		compute->set_variable("bottomColor", bottomGradientColor);
		compute->bind_descriptors();
		compute->dispatch(5, 5, 1);
		compute->end_pass();
	}

	void destroy() {
		delete compute.release();
	}

private:
	Unique<RenderPassCompute> compute;
	CVAR_DEF(Color4, topGradientColor, Color4(146, 236, 197));
    CVAR_DEF(Color4, bottomGradientColor, Color4(256,256,256));
};

void MyGame::start() {
	auto graphics = scene->add_system<GraphicsSystem>(screen_framebuffer.get());
	graphics->add_post_effect<MyPostEffect>();

	resource_database->load_resource_disk<ResourceImage>("../engine/resources/textures/mk_16_16_nature_tileset_json.json");
	auto mesh_texture = resource_database->load_resource_disk<ResourceImage>("../engine/resources/textures/Sprite-0001.json");
	auto collection = resource_database->load_resource_disk<ResourceMesh>("../engine/resources/meshes/primitives.obj");

	auto cube = scene->create_entity("cube");
	cube.AddComponent<ComponentMeshRenderer>(collection, mesh_texture, StringHash("Cube"));

	auto cam = scene->create_entity("camera");
	auto camera = cam.AddComponent<ComponentCamera>();
	auto &trans = cam.GetComponent<ComponentTransform>();
	trans.position = glm::vec3(0.0f, 0.0f, -3.0f);

	graphics->set_main_camera(cam);
}

void MyGame::update(u32 delta_time) {
	scene->update(delta_time);
}

MyGame::~MyGame() {
}
