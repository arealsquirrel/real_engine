
#include "real/resource/resource_shader.hpp"
#include "real/core/instance.hpp"
#include "real/graphics/graphics.hpp"
#include "real/resource/resource.hpp"
#include <gtest/gtest.h>
#include <real/real.hpp>

using namespace real;

TEST(ResourceShader, ShaderFieldSerialization) {
    Graphics::init_backend({});
    Instance *instance = new Instance();
    auto shader = Resource::load<ResourceSerializerType::Disk, ResourceShader>(instance, "../engine/resources/shaders/test_shader.slang.spv");
    
    
    
    delete shader;
    delete instance;
}
