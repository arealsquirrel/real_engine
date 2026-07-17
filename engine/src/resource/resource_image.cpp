
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/container/string_hash.hpp"
#include "real/core/uuid.hpp"
#include "real/graphics/graphics.hpp"
#include "real/resource/resource.hpp"
#include <nlohmann/json_fwd.hpp>
#include <real/resource/resource_image.hpp>
#include <string>
#include <tracy/Tracy.hpp>
#include <utility>
#include <nlohmann/json.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace real {

ResourceImage::ResourceImage(
		Instance *_instance,
		u32 _width, u32 _height,
		ColorFormat _cformat, ImageFormat _iformat, void *data,
		std::map<StringHash, Tile> _tiles)
	: Resource(_instance),
	  cformat(_cformat), iformat(_iformat),
	  width(_width), height(_height), tiles(_tiles) {

	tiles.insert({StringHash("_full_image"), Tile {
		.position = {0,0},
		.dimension = {width, height},
		.name = "_full_image"
	}});
}

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
	
	ZoneScoped

	Path image_path;
	std::map<StringHash, ResourceImage::Tile> tiles;

	if(path.extension().string() == ".json") {
		RL_LOG_TRACE("Loading image from json");
		std::ifstream f(path);
		nlohmann::json js = nlohmann::json::parse(f);
		image_path = path.remove_filename().string() + js["meta"]["image"].get<std::string>();

		if(name.empty() && path.extension() == ".json") {
			name = image_path.filename();
		}

		for(auto something : js["frames"].items()) {
			auto frame = something.value()["frame"];
			ResourceImage::Tile tile;
			if(something.value().contains("name")) {
				tile.name = something.value()["name"].get<std::string>();
			} else {
				tile.name = name + std::to_string(tiles.size());
			}
			tile.position = std::make_pair(frame["x"].get<int>(), frame["y"].get<int>());
			tile.dimension = std::make_pair(frame["w"].get<int>(), frame["h"].get<int>());
			tiles.insert({StringHash(tile.name.c_str()), tile});
		}

	} else {
		image_path = path;
	}

	if(name.empty()) {
		name = path.filename();
	}

	int x,y,n = 0;
	unsigned char *data = stbi_load(image_path.c_str(), &x, &y, &n, STBI_rgb_alpha);
	
	if(data == NULL) {
		RL_LOG_ERROR("stbi error {} on image {}", stbi_failure_reason(), image_path.c_str());
	}

	auto *image = ResourceImage::create(
			instance, (u32)x, (u32)y,
			ColorFormat::RGBA_FLOAT8, ImageFormat::RENDER_ATTACHMENT_COLOR,
			data, 0, tiles).release();
	free(data);

	return register_resource(image, name, UUID(), image_path);
}

}

