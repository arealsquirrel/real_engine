#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "editor_camera.hpp"
#include "panel.hpp"
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/scene/scene.hpp"
#include <optional>
#include <utility>
#include <vector>

namespace editor {

enum class EditorExitReason {
	NotExiting,
	Exit,
	Reload
};

class Editor {
public:
	enum class EditorState {
		Editing,
		Running,
		Paused
	};

public:
	Editor(Shared<real::Instance> _instance, real::ArgParams _params);
	~Editor();

	void load_game(Path path);
	void destroy_game();
	bool render(u32 delta_time);
	

public:
	template<typename T, typename ...Args>
	void add_panel(Args ...args) {
		panels.push_back(std::make_shared<T>(instance, std::forward<Args>(args)...));
	}

private:
	void render_viewport();
	void render_toolbar();
	void render_engine_panel();

private:
	// things made and managed by the editor
	Shared<real::Instance> instance;
	EditorState editor_state;
	real::ArgParams params;
	real::Game *game;
	real::DLLGameLoad game_loader;
	Shared<real::Scene> active_scene;
	Shared<real::Framebuffer> viewport_framebuffer;
	bool exiting {false};
	std::vector<Shared<editor::Panel>> panels;
	EditorCamera camera;

private:
	// things made by the game we might want to take a look at
	Shared<real::GraphicsSystem> graphics_system;
};

}

#endif
