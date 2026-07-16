
#include "real/core/game.hpp"
#include <real/graphics/renderer.hpp>
#include <tracy/Tracy.hpp>

namespace real {

Renderer::Renderer(Instance *_instance, Shared<Window> _window) 
    : Object(_instance), window(_window) {}

Renderer::~Renderer() {
}

void Renderer::attach_camera(Camera &camera) {
	ZoneScoped

	SceneData *data = scene_data->get_data<SceneData>();
	data->view = camera.view;
	data->proj = camera.proj;
}

void Renderer::destroy_renderers() {
	for(auto &r : subrenderers) {
		r->destroy();
		r.reset();
	}
}

SubRenderer::SubRenderer(Instance *_instance, Renderer *_renderer)
	: Object(_instance), renderer(_renderer) {}

PostEffect::PostEffect(Instance *_instance, Renderer *_renderer)
	: Object(_instance), renderer(_renderer) {}

}
