#ifndef EDITOR_PANEL_HPP
#define EDITOR_PANEL_HPP

#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/scene/scene.hpp"

namespace editor {

class Editor;

class Panel {
public:
	Panel(Shared<real::Instance> _instance);
	~Panel();

	virtual void draw() = 0;

	void set_scene(Shared<real::Scene> _scene) { scene = _scene; }

public:
	const real::UUID panelID;

protected:
	Shared<real::Instance> instance;
	Shared<real::Scene> scene;

	friend Editor;
};

}

namespace std {
    template <>
    struct hash<editor::Panel> {
        size_t operator()(const editor::Panel& s) const {
            return s.panelID;
        }
    };
}

#endif
