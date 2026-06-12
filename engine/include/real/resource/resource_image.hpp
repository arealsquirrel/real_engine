#ifndef REALLIB_RESOURCE_IMAGE_HPP
#define REALLIB_RESOURCE_IMAGE_HPP

#include "imgui.h"
#include <real/core/event.hpp>
#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"

namespace real {

typedef void* ImageHandle;

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

/**
 * @brief Holds an image that can be rendered
 */
class REALLIB_EXPORT ResourceImage : public Resource {
RL_OBJECT(ResourceImage, Resource)

protected:
    ResourceImage(
			Instance *_instance,
			u32 _width, u32 _height, 
        	ColorFormat cformat, ImageFormat iformat,
			void *data=nullptr);

public:
    ~ResourceImage();

	/**
	 * @brief Get the handle object for use by the rendering engine
	 * 
	 * @return ImageHandle 
	 */
    virtual ImageHandle get_handle() = 0;

	/**
	 * @brief Get the imgui textureID object for use by imgui
	 * 
	 * @return ImTextureID 
	 */
	virtual ImTextureID get_imgui_textureID() = 0;

public:
	std::pair<u32, u32> get_image_extent();
	ImageFormat get_image_format();
	ColorFormat get_color_format();

protected:
	ColorFormat cformat;
	ImageFormat iformat;
	u32 width;
	u32 height;
};

}

#endif
