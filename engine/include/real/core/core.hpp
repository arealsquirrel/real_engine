#ifndef REALLIB_CORE_HPP
#define REALLIB_CORE_HPP

namespace real {

#define REAL_ENTRY extern Unique<Game> game_entrypoint(Shared<Instance> instance);

/**
 * @brief basic sanity check for the library.
 */
void print_version();

}

#endif
