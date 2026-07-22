#ifndef REALLIB_BUFFER_HPP
#define REALLIB_BUFFER_HPP

#include <cstdint>
#include <real/core/event.hpp>
#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include <cstddef>
#include <vector>

namespace real {

using BufferHandle = void*;

class REALLIB_EXPORT UniformBuffer : public Object {
RL_OBJECT(UniformBuffer, Object)

protected:
    UniformBuffer(Instance *_instance, size_t _size);

public:
    virtual ~UniformBuffer();

public:
    size_t get_size() const { return size; }

    template<typename T>
    T *get_data() {
        return (T*)get_data();
    }

    virtual BufferHandle get_handle() = 0;

    static UniquePointer<UniformBuffer> create(Instance *instance, size_t size);

public:
    virtual void *get_data() = 0;

protected:
    size_t size;
};

class REALLIB_EXPORT IndexBuffer : public Object {
RL_OBJECT(IndexBuffer, Object)

protected:
	IndexBuffer(Instance *_instance, size_t size);

public:
	virtual ~IndexBuffer();

	virtual void upload_data(std::vector<uint32_t> indices) = 0;
	uint32_t get_size() const { return size; }

	static UniquePointer<IndexBuffer> create(Instance *instance, size_t size);

protected:
	uint32_t size;
};

class REALLIB_EXPORT VertexBuffer : public Object {
RL_OBJECT(VertexBuffer, Object)

protected:
	VertexBuffer(Instance *_instance, size_t size);

public:
	virtual ~VertexBuffer();

	virtual void upload_data(char *data, size_t size) = 0;
	uint32_t get_size() const { return size; }

	static UniquePointer<VertexBuffer> create(Instance *instance, size_t size);

protected:
	uint32_t size;
};

}

#endif
