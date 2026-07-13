
#include "real/core/reflection.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace real;

struct MyStruct {
	int x;
	int y[5];
	int z;
	std::vector<float> vec;
};

struct AnotherStruct {
	MyStruct foo;
	std::string bar;
	std::vector<MyStruct> vec;
};

RL_REFLECT(MyStruct, RL_REFLECT_FIELD(x), RL_REFLECT_FIELD(y), RL_REFLECT_FIELD(vec));
RL_REFLECT(AnotherStruct, RL_REFLECT_FIELD(foo), RL_REFLECT_FIELD(vec))

TEST(Reflection, reflect_basic) {
	const reflect::Type *type = Reflection::get_type<MyStruct>();
	EXPECT_STREQ(type->name.c_str(), "MyStruct");
	EXPECT_STREQ(type->fields.at("x").field_name.c_str(), "x");
	EXPECT_STREQ(type->fields.at("y").field_name.c_str(), "y");
}

TEST(Reflection, reflect_type_map) {
	auto &map = real::Reflection::type_map;
	EXPECT_STREQ(map.at(typeid(MyStruct).hash_code())->name.c_str(), "MyStruct");
}

TEST(Reflect, reflect_array) {
	const reflect::Type *type = Reflection::get_type<MyStruct>();
	EXPECT_EQ(type->fields.at("y").array_dims, 5);
	EXPECT_EQ(type->fields.at("y").storage_structure, reflect::StorageStructure::Array);
	EXPECT_EQ(type->fields.at("y").primitive_type, reflect::PrimitiveTypes::t_int);
}

TEST(Reflect, reflect_vector) {
	const reflect::Type *type = Reflection::get_type<AnotherStruct>();
	EXPECT_EQ(type->fields.at("vec").primitive_type, reflect::PrimitiveTypes::t_struct);
	EXPECT_EQ(type->fields.at("vec").storage_structure, reflect::StorageStructure::Vector);
	EXPECT_STREQ(type->fields.at("vec").next->name.c_str(), "MyStruct");
}

