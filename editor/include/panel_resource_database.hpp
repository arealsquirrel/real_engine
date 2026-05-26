#ifndef EDITOR_PANEL_RESOURCE_DATABASE_HPP
#define EDITOR_PANEL_RESOURCE_DATABASE_HPP

#include "panel.hpp"

namespace editor {

class PanelResourceDatabase : public Panel {
public:
	using Panel::Panel;
	~PanelResourceDatabase() = default;

	void draw() override;
};

}

#endif
