#ifndef REALLIB_RENDER_PASS_GEOMETRY_HPP
#define REALLIB_RENDER_PASS_GEOMETRY_HPP

#include <real/core/event.hpp>
#include "real/core/object.hpp"
#include "real/core/vec.hpp"
#include "real/graphics/renderpass.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <optional>

namespace real {

class Framebuffer;

enum class GeometryCullMode {
	NONE,
	FRONT,
	BACK
};

enum class GeometryFrontFace {
	CounterClockwise,
	Clockwise
};

enum class GeometryPolygonMode {
	Fill,
	Line,
	Point
};

enum class GeometryTopology {
	Triangle_list,
	Line_list,
	Point_list
};

enum class MultisamplingCount {
    One,
    Two,
    Four,
    Eight,
    Sixteen
};

/**
 * @brief info for the RenderPassGeometry renderpass.
 */
struct REALLIB_EXPORT RenderPassGeometryInfo {
	bool depth;
	GeometryTopology topology;
	GeometryPolygonMode polygon_mode;
	GeometryFrontFace front_face;
	GeometryCullMode cull_mode;
	MultisamplingCount msaa;
};

/**
 * @brief defines a renderpass that renderers geometry
 */
class REALLIB_EXPORT RenderPassGeometry : public RenderPass {
RL_OBJECT(RenderPassGeometry, RenderPass)

protected:
	RenderPassGeometry(
		Instance *_instance, ShaderLayout layout,
		std::vector<RenderPassResource> _resources)
		: RenderPass(_instance, layout, _resources) {};

public:
	virtual ~RenderPassGeometry() = default;

	/**
	 * @brief beigns a render pass the renders geometry.
	 * 
	 * @param framebuffer the target of this renderpass. the renderpass takes the viewport info from the framebuffer.
	 */
	virtual void begin_pass(Framebuffer *framebuffer) = 0;
	virtual void end_pass() = 0;
	virtual void draw_mesh(ResourceHandle<ResourceMesh> mesh, ResourceMesh::Mesh sub_mesh) = 0;
	virtual void draw_mesh(ResourceHandle<ResourceMesh> mesh) = 0;
	virtual void bind_descriptors() override = 0;
};

}

#endif
