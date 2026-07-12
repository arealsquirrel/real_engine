#ifndef RL_REFLECTION_HPP
#define RL_REFLECTION_HPP

#include "real/core/core.hpp"
#include "real/core/string_hash.hpp"
#include "real/core/types.hpp"
#include <cstddef>
#include <map>
#include <real/core/uuid.hpp>
#include <real/math/math.hpp>
#include <string>
#include <type_traits>
#include <vector>

namespace real {

enum class PrimitiveTypes {
	t_int,
	t_float,
	t_string,
	t_u64,
	t_Vec2,
	t_Vec3,
	t_Vec4,
	t_struct,
	t_vector,
	t_UUID
};


class REALLIB_EXPORT Reflection {
public:
	class Type;

	struct Field {
		std::string field_name;
		u32 offset;
		u32 field_size;
		u32 type_size;
		u32 array_dims;
		PrimitiveTypes type;
		bool is_ptr;
		const Type *next;
	};
	
	struct Type {
		std::string name;
		StringHash hash;
		std::map<std::string, Field> fields;
	};

public:
	template<typename T>
	static const PrimitiveTypes get_type_enum() {
		return PrimitiveTypes::t_struct;
	}

	template<typename T>
	static const Type *type_ptr_reflect() {
		return T::reflect_type_info();
	}
};

#define RL_REFLECT_INTERNAL_PRIM(TypeName) \
template<> \
inline const Reflection::Type *Reflection::type_ptr_reflect<TypeName>() { \
	static Type type; \
	type.hash = StringHash(#TypeName); \
	type.name = std::string(#TypeName); \
	return &type; \
} \
template<> \
inline const PrimitiveTypes Reflection::get_type_enum<TypeName>() { \
	return PrimitiveTypes::t_##TypeName; \
} \
template<> \
inline const Reflection::Type *Reflection::type_ptr_reflect<TypeName *>() { \
	static Type type; \
	type.hash = StringHash(#TypeName); \
	type.name = std::string(#TypeName); \
	return &type; \
} \
template<> \
inline const PrimitiveTypes Reflection::get_type_enum<TypeName *>() { \
	return PrimitiveTypes::t_##TypeName; \
}


#define RL_REFLECT(TypeName, ...) \
	static const real::Reflection::Type *reflect_type_info() { \
		using ReflectedType = TypeName; \
		static real::Reflection::Type type; \
		type.name = #TypeName; \
		type.hash = real::StringHash(type.name); \
		type.fields = {__VA_ARGS__}; \
		return &type; \
	}

template<>
inline const Reflection::Type *Reflection::type_ptr_reflect<std::string>() {
	static Type type;
	type.hash = StringHash("string");
	type.name = std::string("string");
	return &type;
}

template<>
inline const PrimitiveTypes Reflection::get_type_enum<std::string>() {
	return PrimitiveTypes::t_string;
}

RL_REFLECT_INTERNAL_PRIM(u64)
RL_REFLECT_INTERNAL_PRIM(int)
RL_REFLECT_INTERNAL_PRIM(float)
RL_REFLECT_INTERNAL_PRIM(UUID)
RL_REFLECT_INTERNAL_PRIM(Vec2)
RL_REFLECT_INTERNAL_PRIM(Vec3)
RL_REFLECT_INTERNAL_PRIM(Vec4)

template<typename T>
static Reflection::Field reflect_get_field(std::string name, size_t offset) {
	Reflection::Field field;
	field.field_name = name;
	field.offset = offset;
	field.field_size = sizeof(T);
	field.type_size = sizeof(T);
	if constexpr (std::is_array_v<T>) {
		field.array_dims = std::extent_v<T>;
	} else {
		field.array_dims = 1;
	}

	field.type = Reflection::get_type_enum<std::decay_t<T>>();
	field.is_ptr = std::is_pointer_v<T>;
	field.next = Reflection::type_ptr_reflect<std::decay_t<T>>();
	return field;
}

#define RL_REFLECT_FIELD(name) {#name, real::reflect_get_field<decltype(name)>(#name, offsetof(ReflectedType, name)) }

}

#endif
