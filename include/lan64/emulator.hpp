//
// Created by henrik on 20.08.21.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include "lan64/integer.hpp"


namespace LAN64
{

template<typename Record, typename Field>
constexpr std::uintptr_t offset_of(Field (Record::* field))
{
    const Record r{};

    return (reinterpret_cast<std::uintptr_t>(&(r.*field)) - reinterpret_cast<std::uintptr_t>(&r));
}

using n64_size_t = std::uint32_t;

enum struct EmulatorState
{
    STOPPED = 0, //< The emulator is not running or memory is not accessible
    RUNNING,     //< The emulator is running, memory accessible
    PAUSED,      //< The emulator is paused, memory accessible
};

/*
 * Emulator interface
 * Only derive from this class if you know what you're doing
 * You should probably derive from Emulator<T> below!
 * For an example check m64plus.hpp
 */
struct IEmulator
{
    /// Return current emulator state
    virtual EmulatorState state() = 0;

protected:
    /// Read byte from physical address addr, should not do bounds checking, return true on success
    virtual bool read_byte(n64_size_t addr, std::uint8_t& v) = 0;

    /// Write byte to physical address addr, should not do bounds checking, return true on success
    virtual bool write_byte(n64_size_t addr, std::uint8_t v) = 0;

public:
    template<typename T>
    using Casted = std::conditional_t<std::is_enum_v<T>,
        Uint<sizeof(T) * 8>,
        std::conditional_t<std::is_signed_v<T>,
            unsigned_t<T>,
            T>>;

    // All public memory access functions use logical address space

    template<typename T>
    void read(n64_size_t addr, T& v)
    {
        addr = logical_to_physical(addr);

        if(!check_bounds(addr, sizeof(T)))
            throw std::runtime_error("N64 address out of bounds");

        Casted<T> x;
        read_val(addr, x);
        v = static_cast<T>(x);
    }

    template<typename T>
    void write(n64_size_t addr, T v)
    {
        addr = logical_to_physical(addr);

        if(!check_bounds(addr, sizeof(T)))
            throw std::runtime_error("N64 address out of bounds");

        auto x{static_cast<Casted<T>>(v)};
        write_val(addr, x);
    }

    template<typename T>
    void readc(n64_size_t& addr, T& v)
    {
        read(addr, v);
        addr += sizeof(T);
    }

    template<typename T>
    void writec(n64_size_t& addr, T v)
    {
        write(addr, v);
        addr += sizeof(T);
    }

    template<typename Record, typename Field>
    void read_field(n64_size_t struct_addr, Field (Record::* field), Field& v)
    {
        static_assert(std::is_standard_layout_v<Record>);

        read(struct_addr + offset_of(field), v);
    }

    template<typename Record, typename Field>
    void write_field(n64_size_t struct_addr, Field (Record::* field), Field v)
    {
        static_assert(std::is_standard_layout_v<Record>);

        write(struct_addr + offset_of(field), v);
    }

    virtual void read_array(n64_size_t addr, void* buf, n64_size_t len) = 0;
    virtual void write_array(n64_size_t addr, const void* buf, n64_size_t len) = 0;

protected:
    // All these use physical addresses
    virtual void read_val(n64_size_t addr, std::uint8_t& v) = 0;
    virtual void write_val(n64_size_t addr, std::uint8_t v) = 0;
    virtual void read_val(n64_size_t addr, std::uint16_t& v) = 0;
    virtual void write_val(n64_size_t addr, std::uint16_t v) = 0;
    virtual void read_val(n64_size_t addr, std::uint32_t& v) = 0;
    virtual void write_val(n64_size_t addr, std::uint32_t v) = 0;
    virtual void read_val(n64_size_t addr, std::uint64_t& v) = 0;
    virtual void write_val(n64_size_t addr, std::uint64_t v) = 0;
    virtual void read_val(n64_size_t addr, float& v) = 0;
    virtual void write_val(n64_size_t addr, float v) = 0;
    virtual void read_val(n64_size_t addr, double& v) = 0;
    virtual void write_val(n64_size_t addr, double v) = 0;

    /// Checks (physical) address bounds
    static bool check_bounds(n64_size_t addr, n64_size_t len);

    static n64_size_t logical_to_physical(n64_size_t addr);
};

/**
 * Derive from this class to implement the IEmulator
 * @tparam Derived Your derived class
 */
template<typename Derived>
struct Emulator : IEmulator
{
    void read_array(n64_size_t addr, void* buf, n64_size_t len) final;

    void write_array(n64_size_t addr, const void* buf, n64_size_t len) final;

protected:
    void read_val(n64_size_t addr, std::uint8_t& v) final;

    void write_val(n64_size_t addr, std::uint8_t v) final;

    void read_val(n64_size_t addr, std::uint16_t& v) final;

    void write_val(n64_size_t addr, std::uint16_t v) final;

    void read_val(n64_size_t addr, std::uint32_t& v) final;

    void write_val(n64_size_t addr, std::uint32_t v) final;

    void read_val(n64_size_t addr, std::uint64_t& v) final;

    void write_val(n64_size_t addr, std::uint64_t v) final;

    void read_val(n64_size_t addr, float& v) final;

    void write_val(n64_size_t addr, float v) final;

    void read_val(n64_size_t addr, double& v) final;

    void write_val(n64_size_t addr, double v) final;
};

template<typename T>
void Emulator<T>::read_array(n64_size_t addr, void* buf, n64_size_t len)
{
    addr = logical_to_physical(addr);

    if(!check_bounds(addr, len))
        throw std::runtime_error("N64 address out of bounds");

    bool x{true};

    for(n64_size_t i{}; i < len; ++i)
    {
        x &= read_byte(addr + i, reinterpret_cast<std::uint8_t*>(buf)[i]);
    }

    if(!x)
        throw std::runtime_error("Invalid read");
}

template<typename T>
void Emulator<T>::write_array(n64_size_t addr, const void* buf, n64_size_t len)
{
    addr = logical_to_physical(addr);

    if(!check_bounds(addr, len))
        throw std::runtime_error("N64 address out of bounds");

    bool x{true};

    for(n64_size_t i{}; i < len; ++i)
    {
        x &= write_byte(addr + i, reinterpret_cast<const std::uint8_t*>(buf)[i]);
    }

    if(!x)
        throw std::runtime_error("Invalid write");
}

template<typename T>
void Emulator<T>::read_val(n64_size_t addr, std::uint8_t& v)
{
    if(!read_byte(addr, v))
        throw std::runtime_error("Invalid read");
}

template<typename T>
void Emulator<T>::write_val(n64_size_t addr, std::uint8_t v)
{
    if(!write_byte(addr, v))
        throw std::runtime_error("Invalid write");
}

template<typename T>
void Emulator<T>::read_val(n64_size_t addr, std::uint16_t& v)
{
    std::uint8_t buf[2];

    if(!read_byte(addr, buf[0]) || !read_byte(addr + 1, buf[1]))
        throw std::runtime_error("Invalid read");

    v = ((std::uint16_t)buf[0] << 8u) | ((std::uint16_t)buf[1]);
}

template<typename T>
void Emulator<T>::write_val(n64_size_t addr, std::uint16_t v)
{
    std::uint8_t buf[2];

    buf[0] = (std::uint8_t)(v >> 8u);
    buf[1] = (std::uint8_t)v;

    if(!write_byte(addr, buf[0]) || !write_byte(addr + 1, buf[1]))
        throw std::runtime_error("Invalid write");
}

template<typename T>
void Emulator<T>::read_val(n64_size_t addr, std::uint32_t& v)
{
    std::uint16_t buf[2];

    read_val(addr, buf[0]);
    read_val(addr + 2, buf[1]);

    v = ((std::uint32_t)buf[0] << 16u) | ((std::uint32_t)buf[1]);
}

template<typename T>
void Emulator<T>::write_val(n64_size_t addr, std::uint32_t v)
{
    std::uint16_t buf[2];

    buf[0] = (std::uint16_t)(v >> 16u);
    buf[1] = (std::uint16_t)v;

    write_val(addr, buf[0]);
    write_val(addr + 2, buf[1]);
}

template<typename T>
void Emulator<T>::read_val(n64_size_t addr, std::uint64_t& v)
{
    std::uint32_t buf[2];

    read_val(addr, buf[0]);
    read_val(addr + 4, buf[1]);

    v = ((std::uint64_t)buf[0] << 32u) | ((std::uint64_t)buf[1]);
}

template<typename T>
void Emulator<T>::write_val(n64_size_t addr, std::uint64_t v)
{
    std::uint32_t buf[2];

    buf[0] = (std::uint32_t)(v >> 32u);
    buf[1] = (std::uint32_t)v;

    write_val(addr, buf[0]);
    write_val(addr + 4, buf[1]);
}

template<typename T>
void Emulator<T>::read_val(n64_size_t addr, float& v)
{
    std::uint32_t x;
    read_val(addr, x);
    std::memcpy(&v, &x, sizeof(v));
}

template<typename T>
void Emulator<T>::write_val(n64_size_t addr, float v)
{
    std::uint32_t x;
    std::memcpy(&x, &v, sizeof(v));
    write_val(addr, x);
}

template<typename T>
void Emulator<T>::read_val(n64_size_t addr, double& v)
{
    std::uint64_t x;
    read_val(addr, x);
    std::memcpy(&v, &x, sizeof(v));
}

template<typename T>
void Emulator<T>::write_val(n64_size_t addr, double v)
{
    std::uint64_t x;
    std::memcpy(&x, &v, sizeof(v));
    write_val(addr, x);
}

} // LAN64
