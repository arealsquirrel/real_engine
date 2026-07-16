#ifndef REALLIB_OBJECT_HPP
#define REALLIB_OBJECT_HPP

#include "real/core/core.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"

namespace real {

template<typename T>
class Ref;

class Instance;
class EventListener;

struct TypeInfo {
    const char *name;
    const UUID id;
    const TypeInfo *parent;
};

class REALLIB_EXPORT Object {
public:
    explicit Object(Instance *_instance);
    virtual ~Object();

public:
    static const TypeInfo *object_typeinfo_static() { static TypeInfo i{"Object", UUID(), nullptr}; return &i; }
    static const TypeInfo *object_parent_typeinfo_static() { return nullptr; }
    virtual const char *object_name() const { return Object::object_typeinfo_static()->name; };
	virtual const TypeInfo *object_typeinfo() const { return Object::object_parent_typeinfo_static(); }
    UUID get_instance_uuid() const { return object_id; }

	static u32 get_object_count();

protected:
    Instance *instance {nullptr};
    UUID object_id;

private:
	static u32 object_count;

	template<typename T>
	friend class Ref;
};

#define RL_OBJECT(CLASS_NAME, CLASS_PARENT) public: \
    static const TypeInfo *object_typeinfo_static() { static TypeInfo i{#CLASS_NAME, UUID(), CLASS_PARENT::object_parent_typeinfo_static() }; return &i; } \
    static const TypeInfo *object_parent_typeinfo_static() { return CLASS_NAME::object_typeinfo_static()->parent; } \
    virtual const char *object_name() const override { return CLASS_NAME::object_typeinfo()->name; }; \
	virtual const TypeInfo *object_typeinfo() const override { return CLASS_NAME::object_typeinfo_static(); }

}

namespace std {
	template<>
	struct hash<real::Object> {
		std::size_t operator()(const real::Object &object) const {
			return object.get_instance_uuid().uuid;
		}
	};
}

#endif
