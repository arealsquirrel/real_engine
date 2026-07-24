#ifndef REALLIB_RESOURCE_IMAGE_HPP
#define REALLIB_RESOURCE_IMAGE_HPP

#include "imgui.h"
#include <map>
#include <real/core/event.hpp>
#include <string>
#include <utility>
#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/container/string_hash.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"
#include "real/graphics/texture.hpp"

namespace real {

class REALLIB_EXPORT ResourceImage : public Resource {
RL_OBJECT(ResourceImage, Resource)

public:
	struct Tile {
		std::pair<u32, u32> position;
		std::pair<u32, u32> dimension;
		std::string name;
	};

public:
    ResourceImage(
			Instance *_instance,
			u32 _width, u32 _height, 
        	ColorFormat cformat, ImageFormat iformat,
			void *data=nullptr, std::map<StringHash, Tile> tiles={});

public:
    ~ResourceImage();

public:
	std::map<StringHash, Tile> tiles;
	UniquePointer<Texture> texture;
};

}

#endif
