#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "panel.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include <utility>
#include <vector>

namespace editor {

class Editor {
public:
	Editor(Shared<real::Instance> _instance);
	~Editor();

public:
	void render();

	template<typename T, typename ...Args>
	void add_panel(Args ...args) {
		panels.push_back(std::make_shared<T>(instance, std::forward<Args>(args)...));
	}

private:
	Shared<real::Instance> instance;
	std::vector<Shared<editor::Panel>> panels;
};

}

#endif
