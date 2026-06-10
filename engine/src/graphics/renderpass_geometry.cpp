
#include <real/graphics/render_pass_geometry.hpp>

namespace real {

void RenderPassGeometry::set_vieport(Vec2Int _viewport) {
    viewport_size = _viewport;
}

Vec2Int RenderPassGeometry::get_viewport() const {
    return viewport_size;
}

}

