
#include "real/core/core.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_shader.hpp"
#include "real/core/instance.hpp"
#include "real/graphics/graphics.hpp"
#include "real/resource/resource.hpp"
#include <gtest/gtest.h>
#include <real/real.hpp>

using namespace real;

class ResourceShaderTestFixture  : public testing::Test { 
protected:
    ResourceShaderTestFixture() {
        Graphics::init_backend({});
        instance = new Instance();
        compute_shader = instance->resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/test_shader.slang.spv");
        frag_vert_shader = instance->resource_database->load_resource_disk<ResourceShader>( "../engine/resources/shaders/flat.slang.spv");
    }
    
    ~ResourceShaderTestFixture() {
        compute_shader.free();
        frag_vert_shader.free();
        delete instance;
    }

    Instance *instance;
    ResourceHandle<ResourceShader> compute_shader;
    ResourceHandle<ResourceShader> frag_vert_shader;
};

TEST_F(ResourceShaderTestFixture, ShaderTypeSerialization) {
    EXPECT_EQ(compute_shader.get()->get_type(), ShaderTypeFlag_COMPUTE);
    EXPECT_TRUE(CHECK_FLAG(frag_vert_shader.get()->get_type(), ShaderTypeFlag_VERTEX));
    EXPECT_TRUE(CHECK_FLAG(frag_vert_shader.get()->get_type(), ShaderTypeFlag_FRAGMENT));
}

TEST_F(ResourceShaderTestFixture, ShaderComputeSerialization) {
    auto t = compute_shader.get()->get_internal_data().compute_threads;
    EXPECT_EQ(t.x, 16);
    EXPECT_EQ(t.y, 32);
    EXPECT_EQ(t.z, 1);
    EXPECT_EQ(compute_shader.get()->get_layout().fields.size(), 7);
}

TEST_F(ResourceShaderTestFixture, ShaderArraySerialization) {
    auto arr = compute_shader.get()->get_layout().get_field("arr");
    EXPECT_EQ(arr.data_type, ShaderDataType::STRUCT);

    arr = compute_shader.get()->get_layout().get_field("lights");
    EXPECT_EQ(arr.is_array, true);
    EXPECT_EQ(arr.array_size, 32);
    EXPECT_EQ(arr.data_type, ShaderDataType::UNIFORM_BUFFER);
}
