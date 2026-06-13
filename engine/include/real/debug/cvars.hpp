#ifndef REALLIB_CVARS_HPP
#define REALLIB_CVARS_HPP

#include "real/core/core.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include <map>
#include <memory>
#include <string>

namespace real {

enum class REALLIB_EXPORT CVarEditParamers {
    ReadOnly,
    ReadWrite,
};

enum class REALLIB_EXPORT CVarType : u32 {
    Int,
    Color4,
    Float4,
    Float3,
    Float,
};

class REALLIB_EXPORT CVarWrapper {
public:
    CVarWrapper(std::string _name, CVarEditParamers _params)
        : params(_params), name(_name) {};
    virtual ~CVarWrapper() = default;

public:
    virtual void render_imgui() = 0;
    virtual CVarType get_cvar_type() const = 0;

    const CVarEditParamers params;
    const std::string name;
};

template<typename T>
class REALLIB_EXPORT CVar : public CVarWrapper {
public:
    CVar(T init, std::string _name, CVarEditParamers _params)
        : CVarWrapper(_name,  _params), value(init) {}
    ~CVar() override = default;

public:
    static CVarType get_static_cvar_type();
    CVarType get_cvar_type() const override;
    void render_imgui() final override;

public:
    T get_value() const { return value; }
    void set_value(T n) { value = n; }

	void clear_cvars();

private:
    T value;
};

class REALLIB_EXPORT CVarSystem {
private:
    CVarSystem();
    ~CVarSystem();

public:
    void render_imgui();

    template<typename T>
    Shared<CVar<T>> register_cvar(std::string name, T init_value=T(), CVarEditParamers params=CVarEditParamers::ReadWrite) {
        Shared<CVar<T>> var = std::make_shared<CVar<T>>(init_value, name, params);
        cvars.emplace(name, var);
        return var;
    }

	void clear_cvars();
    
public:
    static CVarSystem &get();
    std::map<std::string, Shared<CVarWrapper>> cvars;
};



#define CVAR_DEF_SMALL_EDITPARAMS(type,NAME,init_value,edit_params) Shared<CVar<type>> NAME = ::real::CVarSystem::get().register_cvar<type>(#NAME, init_value, edit_params);
#define CVAR_DEF_INIT_VALUE(type,NAME,init_value) Shared<CVar<type>> NAME = ::real::CVarSystem::get().register_cvar<type>(#NAME, init_value);
#define CVAR_DEF_SMALL(type,NAME) Shared<CVar<type>> NAME = ::real::CVarSystem::get().register_cvar<type>(#NAME);
#define CVAR_GET_MAKE_MACRO(_1, _2, _3, NAME, ...) NAME
#define CVAR_DEF(type, ...) CVAR_GET_MAKE_MACRO(__VA_ARGS__, CVAR_DEF_SMALL_EDITPARAMS, CVAR_DEF_INIT_VALUE, CVAR_DEF_SMALL)(type, __VA_ARGS__)

}

#endif
