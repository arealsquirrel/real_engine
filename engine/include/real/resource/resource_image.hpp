#ifndef REALLIB_RESOURCE_IMAGE_HPP
#define REALLIB_RESOURCE_IMAGE_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"

namespace real {

typedef void* ImageHandle;

enum class ColorFormat {
	UNKNOWN,
    RGB_FLOAT,
};

class ResourceImage : public Resource {
protected:
    ResourceImage(
			Instance *_instance,
			u32 width, u32 height, ColorFormat format, void *data=nullptr);

public:
	static ResourceImage *create(
			Instance *_instance,
			u32 width, u32 height, ColorFormat format, void *data=nullptr);

public:
    ~ResourceImage();

    virtual ImageHandle get_handle() = 0;
	virtual ColorFormat get_format() = 0;
	virtual std::pair<u32, u32> get_image_extent() = 0;
};

}

#endif
