#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "editor_camera.hpp"
#include "editor_gizmos.hpp"
#include "panel.hpp"
#include "real/container/ref.hpp"
#include "real/core/allocator.hpp"
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/scene/scene.hpp"
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
	Editor(real::Ref<real::Instance> _instance, real::ArgParams _params);
	~Editor();

	void load_game(Path path);
	void destroy_game();
	bool render(u32 delta_time);

public:
	void step_game();
	void step_editor();

public:
	template<typename T, typename ...Args>
	void add_panel(real::Allocator *alloc, Args ...args) {
		panels.push_back(real::Ref<Panel>(alloc,  (Panel*)alloc->allocate_object<T>(instance, std::forward<Args>(args)...)));
	}

private:
	void render_viewport();
	void render_toolbar();
	void render_engine_panel();

	void set_editing();
	void set_running();
	void set_paused();

private:
	// things made and managed by the editor
	real::Ref<real::Instance> instance;
	EditorState editor_state;
	real::ArgParams params;
	real::Game *game;
	real::DLLGameLoad game_loader;
	real::Ref<real::Framebuffer> viewport_framebuffer;
	bool exiting {false};
	std::vector<real::Ref<editor::Panel>> panels;
	EditorCamera camera;
	EditorGizmos gizmos;

	real::Ref<real::Scene> active_scene;
	// real::Ref<real::Scene> edited_scene;

private:
	// things made by the game we might want to take a look at
	real::Ref<real::GraphicsSystem> graphics_system;
};

}

#endif
