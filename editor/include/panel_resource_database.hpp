#ifndef EDITOR_PANEL_RESOURCE_DATABASE_HPP
#define EDITOR_PANEL_RESOURCE_DATABASE_HPP

#include "panel.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource_image.hpp"

namespace editor {

class PanelResourceDatabase : public Panel {
public:
	PanelResourceDatabase(real::Ref<real::Instance> _instance);
	~PanelResourceDatabase() = default;

	void draw() override;

public:
	void draw_resource_icon_view();
	void draw_filesystem_view(Path path);
	real::ResourceImage *get_icon_from_path(Path path);

private:
	real::ResourceHandle<real::ResourceImage> icon_file;
	real::ResourceHandle<real::ResourceImage> icon_audio;
	real::ResourceHandle<real::ResourceImage> icon_code;
	real::ResourceHandle<real::ResourceImage> icon_image;
	real::ResourceHandle<real::ResourceImage> icon_zipper;
	real::ResourceHandle<real::ResourceImage> icon_folder;

	bool render_filesystem {true};
	Path project_root;
};

}

#endif
