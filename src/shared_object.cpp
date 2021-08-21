
//
// Created by henrik on 10.04.19
//

#include <cassert>

#include "lan64/shared_object.hpp"

#ifdef __linux__
    #include <dlfcn.h>
#endif


SharedObjectHandle::SharedObjectHandle(const char* file):
    Base(load_shared_object(file))
{}

SharedObjectHandle::SharedObjectHandle(const std::string& file):
    SharedObjectHandle(file.c_str())
{}

void* SharedObjectHandle::load_function(const char* name)
{
    assert(get());

    return ::load_function(get(), name);
}

void* SharedObjectHandle::load_function(const std::string& name)
{
    return load_function(name.c_str());
}


#ifdef __linux__

shared_object_t load_shared_object(const char* file)
{
    return dlopen(file, RTLD_LAZY);
}

shared_object_t shared_object_from_current_proc()
{
    return dlopen(nullptr, RTLD_LAZY);
}

bool unload_shared_object(shared_object_t object)
{
    return (dlclose(object) == 0);
}

void* load_function(shared_object_t object, const char* name)
{
    return dlsym(object, name);
}

std::string get_shared_object_error()
{
    auto msg{dlerror()};
    return {msg ? msg : ""};
}

#elif defined _WIN32

shared_object_t load_shared_object(const char* file)
{
    return LoadLibraryA(file);
}

shared_object_t shared_object_from_current_proc()
{
    shared_object_t module;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)shared_object_from_current_proc, &module);
    return module;
}

bool unload_shared_object(shared_object_t object)
{
    return (FreeLibrary(object) != 0);
}

void* load_function(shared_object_t object, const char* name)
{
    return GetProcAddress(object, name);
}

std::string get_shared_object_error()
{
    return std::to_string(GetLastError());
}

#endif
