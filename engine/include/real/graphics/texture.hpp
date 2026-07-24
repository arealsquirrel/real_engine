#ifndef REALLIB_TEXTURE_HPP
#define REALLIB_TEXTURE_HPP

#include "real/core/instance.hpp"
#include "real/core/object.hpp"
namespace real {

/**
 * @brief the format of the data pointer for colors
 */
enum class ColorFormat {
	UNKNOWN,
	RGBA_FLOAT16,
	RGBA_FLOAT8,
	MONOCHROME,
	DEPTH
};

/**
 * @brief the actual format of the image
 */
enum class ImageFormat {
	UNKNOWN,
	RENDER_ATTACHMENT_COLOR,
	RENDER_ATTACHMENT_DEPTH,
};

class REALLIB_EXPORT Texture : public Object {
RL_OBJECT(Texture, Object)

protected:
    Texture(Instance *_instance,
			u32 _width, u32 _height, 
        	ColorFormat cformat, ImageFormat iformat);

public:
	~Texture() = default;

	virtual void *get_imgui_textureID() = 0;

	/**
	 * uploads data to the pixel buffer
	 */
	virtual void upload_data(void *data, size_t size) = 0;

	std::pair<u32, u32> get_image_extent();
	u32 get_image_width() const { return width; }
	u32 get_image_height() const { return height; }
	ImageFormat get_image_format();
	ColorFormat get_color_format();

	static UniquePointer<Texture> create(
			Instance *_instance, u32 width, u32 height,
			ColorFormat cformat, ImageFormat iformat, int mips=0);

protected:
	ColorFormat cformat;
	ImageFormat iformat;
	u32 width;
	u32 height;
};

}

#endif
