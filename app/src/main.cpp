
#include "real/graphics/window.hpp"
#include <real/core/core.hpp>
#include <real/graphics/render_pass_compute.hpp>
#include <imgui.h>


using namespace real;

constexpr u32 WINDOW_WIDTH = 500;
constexpr u32 WINDOW_HEIGHT = 500;
constexpr const char* APPLICATION_NAME = "game engine wahoo";

static inline void init_instance(Shared<Instance> instance) {
	WindowInfo window_info {
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = APPLICATION_NAME
	};

	instance->init(window_info);
}

void game_main(Shared<Instance> instance) {
	init_instance(instance);

	while(instance->update() == false) {
		
	}
}

