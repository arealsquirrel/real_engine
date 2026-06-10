#ifndef REALLIB_RENDER_PASS_GEOMETRY_HPP
#define REALLIB_RENDER_PASS_GEOMETRY_HPP

#include "real/core/object.hpp"
#include "real/core/vec.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <optional>

namespace real {

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

/**
 * @brief info for the RenderPassGeometry renderpass.
 */
struct REALLIB_EXPORT RenderPassGeometryInfo {
	ResourceHandle<ResourceImage> renderImage;
	std::optional<ResourceHandle<ResourceImage>> depthImage {std::nullopt};

	GeometryTopology topology;
	GeometryPolygonMode polygon_mode;
	GeometryFrontFace front_face;
	GeometryCullMode cull_mode;
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

	virtual void begin_pass() override = 0;
	virtual void end_pass() override = 0;
	virtual void draw_mesh(ResourceHandle<ResourceMesh> mesh) = 0;

	void set_vieport(Vec2Int _viewport);
	Vec2Int get_viewport() const;

protected:
	Vec2Int viewport_size;
};

}

#endif
