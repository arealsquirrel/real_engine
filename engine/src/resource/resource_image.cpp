
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/uuid.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/graphics.hpp"
#include "real/resource/resource.hpp"
#include <real/resource/resource_image.hpp>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace real {

ResourceImage::ResourceImage(
		Instance *_instance,
		u32 _width, u32 _height, ColorFormat _cformat, ImageFormat _iformat, void *data)
	: Resource(_instance),
	  cformat(_cformat), iformat(_iformat),
	  width(_width), height(_height) {}

ResourceImage::~ResourceImage() {}

std::pair<u32, u32> ResourceImage::get_image_extent() {
	return std::make_pair(width, height);
}

ImageFormat ResourceImage::get_image_format() {
	return iformat;
}

ColorFormat ResourceImage::get_color_format() {
	return cformat;
}

template<>
ResourceHandle<ResourceImage> ResourceDatabase::load_resource_disk<>(
	Path path, std::string name) {
	
	RL_INSTRUMENT_FUNCTION

	int x,y,n = 0;
	unsigned char *data = stbi_load(path.c_str(), &x, &y, &n, STBI_rgb_alpha);
	
	if(data == NULL) {
		RL_LOG_ERROR("stbi error {} on image {}", stbi_failure_reason(), path.c_str());
	}

	auto *image = Graphics::create_resource_image(instance, (u32)x, (u32)y, ColorFormat::RGBA_FLOAT8, ImageFormat::RENDER_ATTACHMENT_COLOR, data).release();
	delete data;
	return register_resource(image, name, UUID(), path);
}

}

