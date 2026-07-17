#ifndef REALLIB_BUFFER_HPP
#define REALLIB_BUFFER_HPP

#include <real/core/event.hpp>
#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include <cstddef>

namespace real {

using BufferHandle = void*;

class REALLIB_EXPORT UniformBuffer : public Object {
RL_OBJECT(UniformBuffer, Object)

protected:
    UniformBuffer(Instance *_instance, size_t _size);

public:
    ~UniformBuffer();

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

}

#endif
