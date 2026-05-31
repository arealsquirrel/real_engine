#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "panel.hpp"
#include "real/core/game.hpp"
#include "real/core/types.hpp"
#include <utility>
#include <vector>

namespace editor {

class Editor {
public:
	Editor(Shared<real::Game> _game);
	~Editor();

public:
	void render();

	template<typename T, typename ...Args>
	void add_panel(Args ...args) {
		panels.push_back(std::make_shared<T>(game, std::forward<Args>(args)...));
	}

private:
	Shared<real::Game> game;
	std::vector<Shared<editor::Panel>> panels;
};

}

#endif
