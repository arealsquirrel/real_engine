#ifndef REALLIB_RESOURCE_HANDLE_HPP
#define REALLIB_RESOURCE_HANDLE_HPP

#include "real/core/types.hpp"
#include "real/core/uuid.hpp"

namespace real {

class ResourceDatabase;

/**
 * @brief defines the state of a resource pointer in a resource handle
 */
enum class ResourceState {
	Loaded,

	// it his been removed from memory but the entry is still in the table
	Unloaded,

	// this resource does not delete when the referance count goes to zero
	NeverDelete,

	// the handle does not exist.
	DoesNotExist,

	// count = 1 and that count should be from resource database
	Unreferenced
};

struct REALLIB_EXPORT ResourceHandleControlBlock {
	u32 count;
	ResourceState state;
	UUID id;
};

/**
 * @brief basicly a shared pointer implementation with extra state tracking. Manages a resource* and deletes it once all referances are out of scope.
 * 
 * @tparam T the resource type
 */
template<typename T>
class REALLIB_EXPORT ResourceHandle {
public:
	ResourceHandle() {
		block = new ResourceHandleControlBlock();
		block->count = 1;
		block->state = ResourceState::DoesNotExist;
		block->id = 0;
		db = nullptr;
	}

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
		destroy();
	}

	void operator =(const ResourceHandle &rh) {
		destroy();
		block = rh.block;
		resource = rh.resource;
		db = rh.db;
		block->count++;
	}

public:
	// wierd they are not friends by default
	template <typename U>
    friend class ResourceHandle;

	friend ResourceDatabase;

public:
	T *get() { return resource; }
	const T *get() const { return resource; }

	u32 get_count() { return block->count; }
	ResourceState get_state() { return block->state; }
	void set_state(ResourceState state) { block->state = state; }
	UUID get_uuid() { return block->id; }

	/**
	 * @brief delete the resource if it is not already unloaded
	 */
	void free() {
		if(block->state != ResourceState::Unloaded) {
			block->state = ResourceState::Unloaded;
			delete resource;
		}
	}

private:
	void destroy() {
		block->count -= 1;

		if( block->count == 1 &&
			block->state != ResourceState::Unloaded &&
			block->state != ResourceState::DoesNotExist) {

			block->state = ResourceState::Unreferenced;
		}

		if(block->count == 0) {
			if(block->state != ResourceState::Unloaded && block->state != ResourceState::DoesNotExist) {
				delete resource;
			}

			delete block;
		}
	}

protected:
	T *resource;
	ResourceDatabase *db;
	ResourceHandleControlBlock *block;
};

}

#endif
