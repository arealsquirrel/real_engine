#ifndef REALLIB_ENTITY_HPP
#define REALLIB_ENTITY_HPP

#include "real/core/uuid.hpp"

namespace real {

/**
 * how do we know which entity has which components
 * storing the entities in the archetypes
 * the systems keep track of what archetypes they can iterate over
 * an abstract interface for iterating over the archetypes
 */

using Entity = UUID;

}

#endif