
#include "real/core/game.hpp"
#include "real/resource/resource.hpp"
#include <real/resource/resource_image.hpp>

namespace real {


ResourceImage::ResourceImage(
		Game *_game,
		u32 width, u32 height, ColorFormat format, void *data,
		std::optional<Path> _path)
	: Resource(_game, _path) {}

ResourceImage::~ResourceImage() {}

}

