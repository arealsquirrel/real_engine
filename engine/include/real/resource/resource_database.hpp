#ifndef REALLIB_RESOURCE_DATABASE_HPP
#define REALLIB_RESOURCE_DATABASE_HPP

#include "real/resource/resource.hpp"
#include <real/core/string_hash.hpp>

namespace real {

/**
 * @brief a data structure for storing and retrieving resources by name
 * some resources may depend on other resources so we have to generate a resource tree of sorts,
 * this will happen in a while because this first requires having resources and whatnot
 */
class ResourceDatabase {
public:
    ResourceDatabase(long _max_bytes);
    ~ResourceDatabase();

public:
    StringHash register_resource(Resource *resource);
    void delete_resource(StringHash handle);
    Resource *query_resource(StringHash handle);

public:
    const long max_bytes;
};

}

#endif