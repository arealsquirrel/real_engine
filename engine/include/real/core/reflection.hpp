#ifndef RL_REFLECTION_HPP
#define RL_REFLECTION_HPP

#include "imgui.h"
#include "real/core/core.hpp"
#include "real/core/string_hash.hpp"
#include "real/core/templates.hpp"
#include "real/core/types.hpp"
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <map>
#include <real/core/uuid.hpp>
#include <real/math/math.hpp>
#include <string>
#include <type_traits>
#include <vector>

/**
 * I SHOULD BURN IN HELL, I SHOULD BURN IN HELL
 * I DONT DESERVE THIS, NOBODY DESERVES THIS.
 *
 * IF YOU WANT COMPILE TIME REFLECTION, IF YOU WANT ME TO MAKE
 * GOOD REFLECTION
 *
 * YOU BETER COME OVER WITH A CRUCIFIX
 * YOU WILL HAVE TO NAIL ME DOWN.
 */

namespace real {

namespace reflect {

class Type;

enum class REALLIB_EXPORT PrimitiveTypes {
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

enum class REALLIB_EXPORT StorageStructure {
	Instance,
	Array,
	Vector
};

struct REALLIB_EXPORT Field {
	std::string field_name;
	u32 offset;
	u32 field_size;
	u32 type_size;
	u32 array_dims;
	PrimitiveTypes primitive_type;
	u32 modifiers;
	bool is_ptr;
	StorageStructure storage_structure;
	const Type *next;
};

struct REALLIB_EXPORT Type {
	std::string name;
	StringHash hash;
	std::map<std::string, Field> fields;
};

template<typename T>
static const PrimitiveTypes get_type_enum() {
	return PrimitiveTypes::t_struct;
}

template<typename T>
static const Type *type_ptr_reflect() {
	return nullptr;
}

// we are in the real namespace right now BTW
class REALLIB_EXPORT Reflection {
public:
	template<typename T>
	static const reflect::Type *get_type() {
		auto at = type_map.find(typeid(T).hash_code());
		if(at == type_map.end()) {
			auto *type = reflect::type_ptr_reflect<T>(); // reflect TS into the map
			return type;
		}

		return at->second;
	}

	static const reflect::Type *get_type(size_t type_id) {
		return type_map.at(type_id);
	}

	template<typename T>
	bool is_type_in_map() {
		return (type_map.count(typeid(T).hash_code()) != 0);
	}

public:
	static std::map<size_t, const reflect::Type*> type_map;
};

template<typename T>
inline static Field reflect_get_field(std::string name, size_t offset, size_t element_size) {
	Field field;
	field.field_name = name;
	field.offset = offset;
	field.field_size = element_size;
	field.is_ptr = std::is_pointer_v<T>;
	field.storage_structure = StorageStructure::Instance;

	if constexpr (std::is_array_v<T>) {
		field.array_dims = std::extent_v<T>;
		field.next = Reflection::get_type<std::remove_all_extents_t<T>>();
		field.primitive_type = get_type_enum<std::remove_all_extents_t<T>>();
		field.type_size = sizeof(std::remove_all_extents_t<T>);
		field.storage_structure = StorageStructure::Array;
		return field;
	} else if constexpr (is_specialization<T, std::vector>::value) {
		field.array_dims = 1;
		field.next = Reflection::get_type<std_vector_type<T>>();
		field.primitive_type = get_type_enum<std_vector_type<T>>();
		field.storage_structure = StorageStructure::Vector;
		field.type_size = sizeof(std_vector_type<T>);
		return field;
	} else {
		field.array_dims = 1;
		field.primitive_type = get_type_enum<T>();
		field.next = Reflection::get_type<T>();
		field.type_size = sizeof(T);
		return field;
	}
}

template<>
inline const PrimitiveTypes get_type_enum<std::string>() {
	return PrimitiveTypes::t_string;
}

class FieldView;

class REALLIB_EXPORT TypeReflector {
public:
	TypeReflector(const Type *_type, char *_view, size_t _offset=0)
		: view(_view), type(_type), offset(_offset) {}
	~TypeReflector() = default;

public:
	FieldView view_field(std::string name);
	bool has_field(std::string name);

public:
	char *view;
	const reflect::Type *type;
	size_t offset;
};

class REALLIB_EXPORT FieldView {
public:
	FieldView(const reflect::Field &_field, char *_view, size_t _offset=0)
		: field(_field), view(_view), offset(_offset) {}
	~FieldView() = default;

	template<typename R>
	void set(const R &value) {
		void *ptr = view+offset+field.offset;
		((R)ptr) = value; // woah
	}

	template<typename R>
	R *get() {
		void *ptr = view+offset+field.offset;
		return (R*)ptr;
	}

	TypeReflector next();
	bool has_next();

public:
	const reflect::Field field;
	char *view;
	size_t offset;
};

#define RL_REFLECT(TypeName, ...) \
	template<> \
	const real::reflect::Type *real::reflect::type_ptr_reflect<TypeName>() { \
		using ReflectedType = TypeName; \
		static real::reflect::Type type; \
		type.name = #TypeName; \
		type.hash = real::StringHash(type.name); \
		type.fields = {__VA_ARGS__}; \
		real::reflect::Reflection::type_map.emplace(typeid(TypeName).hash_code(), &type); \
		return &type; \
	}

#define RL_REFLECT_FIELD(name) {#name, real::reflect::reflect_get_field<decltype(ReflectedType::name)>(#name, offsetof(ReflectedType, name), sizeof(ReflectedType::name)) }

#define RL_REFLECT_INTERNAL_PRIM(TypeName) \
template<> \
inline const PrimitiveTypes get_type_enum<TypeName>() { \
	return PrimitiveTypes::t_##TypeName; \
}

RL_REFLECT_INTERNAL_PRIM(u64)
RL_REFLECT_INTERNAL_PRIM(int)
RL_REFLECT_INTERNAL_PRIM(float)
RL_REFLECT_INTERNAL_PRIM(UUID)
RL_REFLECT_INTERNAL_PRIM(Vec2)
RL_REFLECT_INTERNAL_PRIM(Vec3)
RL_REFLECT_INTERNAL_PRIM(Vec4)

}

template<typename T>
REALLIB_EXPORT
const reflect::Type *reflect_type() {
	return reflect::Reflection::get_type<T>();
}

template<typename T>
REALLIB_EXPORT
reflect::TypeReflector reflect_view_type(T *type) {
	return reflect::TypeReflector(reflect_type<T>(), (char*)type);
}

/** 
 * draws out the fields of this type with imgui
 */
REALLIB_EXPORT
void reflect_imgui(reflect::TypeReflector refl, ImGuiID id);

}

#endif
