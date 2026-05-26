#ifndef REALLIB_RESOURCE_IMAGE_HPP
#define REALLIB_RESOURCE_IMAGE_HPP

#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"

namespace real {

typedef void* ImageHandle;

enum class ColorFormat {
	UNKNOWN,
    RGB_FLOAT,
	RGBA_FLOAT,
	MONOCHROME,
};

enum class ImageFormat {
	
};

class ResourceImage : public Resource {
RL_CLASS(ResourceImage)

protected:
    ResourceImage(
			Instance *_instance,
			u32 width, u32 height, ColorFormat format, void *data=nullptr,
			std::optional<Path> _path=std::nullopt);

public:
	static ResourceImage *create(
			Instance *_instance,
			u32 width, u32 height, ColorFormat format, void *data=nullptr);

public:
    ~ResourceImage();

	virtual ImageFormat get_image_format() = 0;
	virtual void transition_image(FrameContext context, ImageFormat to) = 0;
    virtual ImageHandle get_handle() = 0;
	virtual ColorFormat get_color_format() = 0;
	virtual std::pair<u32, u32> get_image_extent() = 0;
};

}

#endif
