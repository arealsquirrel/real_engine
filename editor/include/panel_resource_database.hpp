#ifndef EDITOR_PANEL_RESOURCE_DATABASE_HPP
#define EDITOR_PANEL_RESOURCE_DATABASE_HPP

#include "panel.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"

namespace editor {

class PanelResourceDatabase : public Panel {
public:
	PanelResourceDatabase(Shared<real::Instance> _instance);
	~PanelResourceDatabase() = default;

	void draw() override;

private:
	real::ResourceHandle<real::ResourceImage> editor_icons;
	real::ResourceImage::Tile file_tile;
};

}

#endif
