#ifndef REALLIB_RENDER_PASS_HPP
#define REALLIB_RENDER_PASS_HPP

#include "real/core/instance.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_shader.hpp"
#include <cstddef>
#include <real/graphics/graphics.hpp>
#include <string>

namespace real {

typedef void* RenderPassData;
typedef char* PushConstantBuffer;

class Renderer;

/**
 * @brief defines the abstract thingy mabob for actualy rendering shit to the screen
 * 
 */
class RenderPass {
public:
    RenderPass(Instance *_instance, ShaderLayout layout,
		std::vector<ResourceHandle<ResourceImage>> _inResources,
		std::vector<ResourceHandle<ResourceImage>> _outResources)
		: instance(_instance), 
		inResources(_inResources), outResources(_outResources),
		shader_layout(layout) {};

    virtual ~RenderPass() = default;

    virtual void begin_pass(FrameContext context) = 0;
    virtual void end_pass(FrameContext context) = 0;

	template<typename T>
	void set_variable(std::string name, T value) {
		set_variable<T>(shader_layout.get_field(name), value);
	}

	template<typename T>
	void set_variable(ShaderField field, T t) {
		set_variable(field, reinterpret_cast<char*>(&t), sizeof(T));
	}

	virtual void set_variable(ShaderField field, char *data, size_t size) = 0;

public:
	const std::vector<ResourceHandle<ResourceImage>> inResources;
	const std::vector<ResourceHandle<ResourceImage>> outResources;

protected:
    Instance *instance;
	ShaderLayout shader_layout;
};

}

#endif
