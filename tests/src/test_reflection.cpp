
#include "real/core/reflection.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/math/vec2.hpp"
#include "real/math/vec3.hpp"
#include "real/math/vec4.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace real;

struct MyStruct {
	int x;
	int y[5];
	int z;
	int *w;

	RL_REFLECT(MyStruct, RL_REFLECT_FIELD(x), RL_REFLECT_FIELD(y), RL_REFLECT_FIELD(w));
};

struct Foo {
	int rawr;
	MyStruct my;

	RL_REFLECT(Foo, RL_REFLECT_FIELD(rawr), RL_REFLECT_FIELD(my));
};

struct AllPrims {
	int i;
	float f;
	std::string str;
	u64 big_number;
	UUID id;
	Vec2 vec2;
	Vec3 vec3;
	Vec4 vec4;

	RL_REFLECT(AllPrims,
		RL_REFLECT_FIELD(i),
		RL_REFLECT_FIELD(f),
		RL_REFLECT_FIELD(str),
		RL_REFLECT_FIELD(big_number),
		RL_REFLECT_FIELD(id),
		RL_REFLECT_FIELD(vec2),
		RL_REFLECT_FIELD(vec3),
		RL_REFLECT_FIELD(vec4),
	);
};

TEST(Reflection, reflect_basic) {
	const Reflection::Type *type = MyStruct::reflect_type_info();
	EXPECT_STREQ(type->name.c_str(), "MyStruct");
	EXPECT_STREQ(type->fields.at("x").field_name.c_str(), "x");
	EXPECT_STREQ(type->fields.at("y").field_name.c_str(), "y");
	EXPECT_EQ(type->fields.at("y").array_dims, 5);
	EXPECT_EQ(type->fields.at("w").type, PrimitiveTypes::t_int);
	EXPECT_EQ(type->fields.at("w").type_size, sizeof(int*));
}

TEST(Reflection, reflect_nested) {
	const Reflection::Type *type = Foo::reflect_type_info();
	EXPECT_STREQ(type->fields.at("my").field_name.c_str(), "my");
	EXPECT_EQ(type->fields.at("my").type, PrimitiveTypes::t_struct);
	EXPECT_STREQ(type->fields.at("my").next->name.c_str(), "MyStruct");
}

TEST(Reflection, reflect_all_prims) {
	const Reflection::Type *type = AllPrims::reflect_type_info();
	EXPECT_EQ(type->fields.at("i").type_size, sizeof(int));
	EXPECT_EQ(type->fields.at("i").type, PrimitiveTypes::t_int);

	EXPECT_EQ(type->fields.at("f").type_size, sizeof(float));
	EXPECT_EQ(type->fields.at("f").type, PrimitiveTypes::t_float);

	EXPECT_EQ(type->fields.at("str").type_size, sizeof(std::string));
	EXPECT_EQ(type->fields.at("str").type, PrimitiveTypes::t_string);

	EXPECT_EQ(type->fields.at("big_number").type_size, sizeof(u64));
	EXPECT_EQ(type->fields.at("big_number").type, PrimitiveTypes::t_u64);

	EXPECT_EQ(type->fields.at("id").type_size, sizeof(UUID));
	EXPECT_EQ(type->fields.at("id").type, PrimitiveTypes::t_UUID);

	EXPECT_EQ(type->fields.at("vec2").type_size, sizeof(Vec2));
	EXPECT_EQ(type->fields.at("vec2").type, PrimitiveTypes::t_Vec2);

	EXPECT_EQ(type->fields.at("vec3").type_size, sizeof(Vec3));
	EXPECT_EQ(type->fields.at("vec3").type, PrimitiveTypes::t_Vec3);

	EXPECT_EQ(type->fields.at("vec4").type_size, sizeof(Vec4));
	EXPECT_EQ(type->fields.at("vec4").type, PrimitiveTypes::t_Vec4);
}

