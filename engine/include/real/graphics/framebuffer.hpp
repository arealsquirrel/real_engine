#ifndef REALLIB_GRAPHICS_FRAMEBUFFER
#define REALLIB_GRAPHICS_FRAMEBUFFER

#include "real/container/color.hpp"
#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/resource/resource_image.hpp"
#include <real/core/core.hpp>
#include <real/core/types.hpp>
#include <real/graphics/renderpass_geometry.hpp>

namespace real {

class REALLIB_EXPORT Framebuffer : public Object {
RL_OBJECT(Framebuffer, Object)

protected:
    Framebuffer(
        Instance *instance, u32 width, u32 height,
        bool depth=true, MultisamplingCount count=MultisamplingCount::One);

public:
    virtual ~Framebuffer();

    virtual ResourceHandle<ResourceImage> get_depth_image() = 0;
    virtual ResourceHandle<ResourceImage> get_msaa_color_image() = 0;
    virtual ResourceHandle<ResourceImage> get_color_resolve_image() = 0;

    virtual void bind() = 0;
    virtual void clear_image(Color4 col={0.0f, 0.0f, 0.0f, 0.0f}) = 0;
    virtual void unbind() = 0;

    static Unique<Framebuffer> create(
        Instance *instance, u32 width, u32 height,
        bool depth=true, MultisamplingCount count=MultisamplingCount::One);

public:
    u32 get_width() const { return width; }
    u32 get_height() const { return height; }
    bool has_depth() const { return depth; }
    MultisamplingCount get_msaa() const { return msaa; }

protected:
    u32 width;
    u32 height;
    bool depth;
    MultisamplingCount msaa;
};

}

#endif
