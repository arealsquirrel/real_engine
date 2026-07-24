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
	EditorGizmos(real::Ref<real::Instance> _instance);
	~EditorGizmos();

	void draw_gizmos(real::Ref<real::Scene> scene);

private:
	real::ResourceHandle<real::ResourceImage> icon_camera;
	real::ResourceHandle<real::ResourceImage> icon_sun_light;
	real::Ref<real::SpriteRenderer> sprite_renderer;
	real::Ref<real::Instance> instance;
};

}

#endif
