
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/graphics.hpp"
#include "real/resource/resource.hpp"
#include <real/resource/resource_image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace real {

ResourceImage::ResourceImage(
		Instance *_instance,
		u32 width, u32 height, ColorFormat _cformat, ImageFormat _iformat, void *data,
		std::optional<Path> _path)
	: Resource(_instance, _path), cformat(_cformat), iformat(_iformat) {}

ResourceImage::~ResourceImage() {}

template<>
ResourceImage *Resource::load<ResourceSerializerType::Disk>(
		Instance *instance, Optional<Path> path) {

	int x,y,n = 0;
	unsigned char *data = stbi_load(path->c_str(), &x, &y, &n, STBI_rgb_alpha);
	
	if(data == NULL) {
		RL_LOG_ERROR("stbi error {} on image {}", stbi_failure_reason(), path->c_str());
	}

	RL_LOG_INFO("{} {} {}", x, y, n);
	auto *image = Graphics::create_resource_image(instance, (u32)x, (u32)y, ColorFormat::RGBA_FLOAT8, ImageFormat::RENDER_ATTACHMENT_COLOR, data).release();
	delete data;
	return image;
}

}

