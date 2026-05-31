#ifndef REALLIB_RENDER_PASS_GEOMETRY_HPP
#define REALLIB_RENDER_PASS_GEOMETRY_HPP

#include "real/core/object.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/renderer.hpp"

namespace real {

struct REALLIB_EXPORT RenderPassGeometryInfo {

};

class REALLIB_EXPORT RenderPassGeometry : public RenderPass {
RL_OBJECT(RenderPassGeometry, RenderPass)

protected:
	RenderPassGeometry(
		Game *_game, ShaderLayout layout,
		std::vector<RenderPassResource> _resources)
		: RenderPass(_game, layout, _resources) {};

public:
	virtual ~RenderPassGeometry() = default;

	virtual void begin_pass(FrameContext context) = 0;
	virtual void end_pass(FrameContext context) = 0;
};

}

#endif
