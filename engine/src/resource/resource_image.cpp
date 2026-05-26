
#include "real/resource/resource.hpp"
#include <real/resource/resource_image.hpp>

namespace real {


ResourceImage::ResourceImage(
		Instance *_instance,
		u32 width, u32 height, ColorFormat format, void *data,
		std::optional<Path> _path)
	: Resource(_instance, _path) {}

ResourceImage::~ResourceImage() {}

}

