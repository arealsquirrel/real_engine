#ifndef EDITOR_PANEL_RESOURCE_VIEWER_HPP
#define EDITOR_PANEL_RESOURCE_VIEWER_HPP

#include "panel.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_handle.hpp"

namespace editor {

class PanelResourceViewer : public Panel {
public:
	PanelResourceViewer(Shared<real::Instance> _instance, 
		std::optional<real::ResourceHandle<real::Resource>> _current_handle = std::nullopt);

	~PanelResourceViewer();

	void draw();

public:
	// what a wonderful language
	std::optional<real::ResourceHandle<real::Resource>> current_handle = std::nullopt;
};

}

#endif
