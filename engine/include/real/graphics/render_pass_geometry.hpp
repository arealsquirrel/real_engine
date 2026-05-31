#ifndef REALLIB_RENDER_PASS_GEOMETRY_HPP
#define REALLIB_RENDER_PASS_GEOMETRY_HPP

#include "real/core/object.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <optional>

namespace real {

struct REALLIB_EXPORT RenderPassGeometryInfo {
	ResourceHandle<ResourceImage> renderImage;
	std::optional<ResourceHandle<ResourceImage>> depthImage {std::nullopt};
};

class REALLIB_EXPORT RenderPassGeometry : public RenderPass {
RL_OBJECT(RenderPassGeometry, RenderPass)

protected:
	RenderPassGeometry(
		Instance *_instance, ShaderLayout layout,
		std::vector<RenderPassResource> _resources)
		: RenderPass(_instance, layout, _resources) {};

public:
	virtual ~RenderPassGeometry() = default;

	virtual void begin_pass(FrameContext context) override = 0;
	virtual void end_pass(FrameContext context) override = 0;
	virtual void draw_mesh(FrameContext context, ResourceHandle<ResourceMesh> mesh) = 0;
};

}

#endif
