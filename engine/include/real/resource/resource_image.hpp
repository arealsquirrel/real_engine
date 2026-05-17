#ifndef REALLIB_RESOURCE_IMAGE_HPP
#define REALLIB_RESOURCE_IMAGE_HPP

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"
#include <optional>

namespace real {

typedef void* ImageHandle;

enum class ColorFormat {
    RGB_FLOAT
};

class ResourceImage : public Resource {
public:
    ResourceImage(Instance *_instance, std::optional<Path> path, u32 width, u32 height, ColorFormat format, void *data=nullptr);
    ~ResourceImage();

    ImageHandle get_handle() { return handle; }

private:
    ImageHandle handle;
};

}

#endif