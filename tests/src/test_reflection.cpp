
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
	std::string bar;
	MyStruct foo;
	std::vector<MyStruct> vec;
};

RL_REFLECT(MyStruct, RL_REFLECT_FIELD(x), RL_REFLECT_FIELD(y), RL_REFLECT_FIELD(z), RL_REFLECT_FIELD(vec));
RL_REFLECT(AnotherStruct, RL_REFLECT_FIELD(foo), RL_REFLECT_FIELD(vec))

TEST(Reflection, reflect_basic) {
	const reflect::Type *type = reflect::Reflection::get_type<MyStruct>();
	EXPECT_STREQ(type->name.c_str(), "MyStruct");
	EXPECT_STREQ(type->fields.at("x").field_name.c_str(), "x");
	EXPECT_STREQ(type->fields.at("y").field_name.c_str(), "y");
}

TEST(Reflect, reflect_array) {
	const reflect::Type *type = reflect::Reflection::get_type<MyStruct>();
	EXPECT_EQ(type->fields.at("y").array_dims, 5);
	EXPECT_EQ(type->fields.at("y").storage_structure, reflect::StorageStructure::Array);
	EXPECT_EQ(type->fields.at("y").primitive_type, reflect::PrimitiveTypes::t_int);
}

TEST(Reflect, reflect_vector) {
	const reflect::Type *type = reflect::Reflection::get_type<AnotherStruct>();
	EXPECT_EQ(type->fields.at("vec").primitive_type, reflect::PrimitiveTypes::t_struct);
	EXPECT_EQ(type->fields.at("vec").storage_structure, reflect::StorageStructure::Vector);
	EXPECT_STREQ(type->fields.at("vec").next->name.c_str(), "MyStruct");
}

TEST(Reflect, reflect_get_field) {
	MyStruct str;
	str.x = 10;
	str.y[3] = 3;
	str.z = 5;
	str.vec.push_back(4.2);
	str.vec.push_back(1.2);
	str.vec.push_back(9.2);

	{
	const reflect::Type *type = reflect::Reflection::get_type<MyStruct>();
	auto fview = reflect::FieldView(type->fields.at("y"), (char*)&str, 0);
	int *arr = fview.get<int>();
	EXPECT_EQ(arr[3], 3);
	}

	{
	const reflect::Type *type = reflect::Reflection::get_type<MyStruct>();
	auto fview = reflect::FieldView(type->fields.at("vec"), (char*)&str, 0);
	auto *vec = fview.get<std::vector<float>>();
	EXPECT_FLOAT_EQ(vec->at(1), 1.2);
	EXPECT_EQ(vec->size(), 3);
	}

	{
	const reflect::Type *type = reflect::Reflection::get_type<MyStruct>();
	auto fview = reflect::FieldView(type->fields.at("x"), (char*)&str, 0);
	EXPECT_EQ(*fview.get<int>(), 10);
	}

	{
	const reflect::Type *type = reflect::Reflection::get_type<MyStruct>();
	auto fview = reflect::FieldView(type->fields.at("z"), (char*)&str, 0);
	EXPECT_EQ(*fview.get<int>(), 5);
	*(fview.get<int>()) = 4;
	}
}

TEST(Reflect, reflect_view_recursive) {
	AnotherStruct another;
	const reflect::Type *type = reflect::Reflection::get_type<AnotherStruct>();
	auto fview = reflect::FieldView(type->fields.at("foo"), (char*)&another, 0);
	fview.get<MyStruct>()->x = 5;
	int *i = fview.next().view_field("x").get<int>();
	EXPECT_EQ(another.foo.x, 5);
}

