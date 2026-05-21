#ifndef REALLIB_RESOURCE_HANDLE_HPP
#define REALLIB_RESOURCE_HANDLE_HPP

#include "real/core/types.hpp"
#include "real/core/uuid.hpp"

namespace real {

class ResourceDatabase;

enum class ResourceState {
	Loaded,

	// it his been removed from memory but the entry is still in the table
	Unloaded,

	// this resource does not delete when the referance count goes to zero
	NeverDelete
};

struct ResourceHandleControlBlock {
	u32 count;
	ResourceState state;
	UUID id;
};

template<typename T>
class ResourceHandle {
public:
	ResourceHandle(
			ResourceDatabase *_db, T *_resource,
			ResourceState _state, UUID uuid)
		: db(_db), resource(_resource) {

		block = new ResourceHandleControlBlock;
		block->count = 1;
		block->state = _state;
		block->id = uuid;
	}

	ResourceHandle(const ResourceHandle<T> &rh)
	 	: db(rh.db), block(rh.block), resource(rh.resource) {
		
		block->count += 1;
	}

	/* casting between resource types WITH NO TYPE CHECKING WHAHAHAHAH */
	template<typename R>
	ResourceHandle(ResourceHandle<R> &rh)
		: db(rh.db), block(rh.block) {
		
		block->count += 1;
		resource = static_cast<T*>(rh.get());
	}

	~ResourceHandle() {
		block->count -= 1;
		if(block->count == 0) {
			if(block->state != ResourceState::Unloaded) {
				delete resource;
			}

			delete block;
		}
	}

public:
	// wierd they are not friends by default
	template <typename U>
    friend class ResourceHandle;

public:
	T *get() { return resource; }
	const T *get() const { return resource; }

	u32 get_count() { return block->count; }
	ResourceState get_state() { return block->state; }
	void set_state(ResourceState state) { block->state = state; }
	UUID get_uuid() { return block->id; }

	void unload() {
		block->state = ResourceState::Unloaded;
		delete resource;
	}

protected:
	T *resource;
	ResourceDatabase *db;
	ResourceHandleControlBlock *block;
};

}

#endif
