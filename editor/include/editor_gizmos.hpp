#ifndef REALLIB_EDITOR_GIZMOS_HPP
#define REALLIB_EDITOR_GIZMOS_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/scene/scene.hpp"
namespace editor {

class EditorGizmos {
public:
	EditorGizmos(Shared<real::Instance> _instance);
	~EditorGizmos();

	void draw_gizmos(Shared<real::Scene> scene);

private:
	real::ResourceHandle<real::ResourceImage> icon_camera;
	Shared<real::SpriteRenderer> sprite_renderer;
	Shared<real::Instance> instance;
};

}

#endif
