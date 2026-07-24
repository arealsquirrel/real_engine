
#include <real/graphics/texture.hpp>
#include <utility>

namespace real {

Texture::Texture(Instance *_instance,
			u32 _width, u32 _height, 
        	ColorFormat _cformat, ImageFormat _iformat) 
	: Object(_instance), width(_width), height(_height), cformat(_cformat), iformat(_iformat) {}

std::pair<u32, u32> Texture::get_image_extent() {
	return std::make_pair(width, height);
}

ImageFormat Texture::get_image_format() { return iformat; }
ColorFormat Texture::get_color_format() { return cformat; };

}

