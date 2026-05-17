#ifndef REALLIB_RESOURCE_HPP
#define REALLIB_RESOURCE_HPP

#include "real/core/instance.hpp"
#include <cstdint>
#include <optional>
#include <real/core/types.hpp>

namespace real {

enum class ResourceSerializerType {
    Disk,
    ZipFile,
};

/**
 * @brief stores data in a specified format so the engine can use it
 */
class Resource {
public:
    Resource(Instance *_instance, Optional<Path> _path);
    virtual ~Resource();

private:
    Instance *instance;

protected:
    Optional<Path> path;
    uint32_t size;
};

// just say you hate fun :3
using LoadStruct = void*;
using SaveStruct = void*;
using SaveObject = void*;

/**
 * @brief an interface for loading and saving
 * a resource type from various intermediate types
 * 
 * @tparam SerializerType the format of the file/directory from where the resource will be loaded or saved
 */
template<ResourceSerializerType SerializerType>
class ResourceSerializer {
public:

    /**
     * @brief loads a resource from data
     * 
     * @tparam ResourceType The resource you would like to load
     * @param loadstruct see the loadStruct for the 
     * resource type you wish to load
     * passes in metadata on how to make your resource
     * @return ResourceType* returns a pointer to the newly created resource!
     */
    template<typename ResourceType>
    static ResourceType *load(
        Instance *instance,
        Optional<Path> path=std::nullopt,
        const LoadStruct loadstruct=nullptr);

    /**
    * @brief saves a resource to another medium/data format
    * 
    * @tparam ResourceType the type of the resource you are saving
    * @param resource a pointer to the resource you want to save
    * @param saveStruct a pointer to a structure for aditional saving information
    * @param path if a path is needed, you can inlcude a path to were you want the resource saved
    * @return SaveObject if you expect additional data once saved, it will be returned here
    */
    template<typename ResourceType>
    static SaveObject save(
        ResourceType *resource,
        const SaveStruct saveStruct=nullptr,
        Optional<Path> path=std::nullopt);
};

}

#endif