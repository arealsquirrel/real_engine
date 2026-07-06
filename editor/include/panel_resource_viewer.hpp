#ifndef EDITOR_PANEL_RESOURCE_VIEWER_HPP
#define EDITOR_PANEL_RESOURCE_VIEWER_HPP

#include "panel.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <string>

namespace editor {

class PanelResourceViewer : public Panel {
public:
	PanelResourceViewer(Shared<real::Instance> _instance, 
		std::optional<real::ResourceHandle<real::Resource>> _current_handle = std::nullopt);

	~PanelResourceViewer();

	void draw();

private:
	template<typename T>
	void display(T *resource);

public:
	// what a wonderful language
	std::string name;
	std::optional<real::ResourceHandle<real::Resource>> current_handle = std::nullopt;

	real::ResourceHandle<real::ResourceImage> editor_icons;
};

}

#endif
