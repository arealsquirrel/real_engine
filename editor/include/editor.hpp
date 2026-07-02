#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "panel.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/camera.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
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
	Editor(Shared<real::Instance> _instance);
	~Editor();

public:
	EditorExitReason render();

	template<typename T, typename ...Args>
	void add_panel(Args ...args) {
		panels.push_back(std::make_shared<T>(instance, std::forward<Args>(args)...));
	}

private:
	void viewport();

private:
	Shared<real::Instance> instance;
	std::vector<Shared<editor::Panel>> panels;
	real::Camera editor_camera;

public:
	real::ResourceHandle<real::ResourceImage> editor_viewport;
};

}

#endif
