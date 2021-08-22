//
// Created by henrik on 20.08.21.
//

#pragma once

#include "lan64/emulator.hpp"
#include "lan64/shared_object.hpp"


namespace LAN64
{

/**
 * Implementation of the IEmulator interface for Mupen64Plus
 * Include "m64p_types.h" before including this file!
 */
struct M64PlusEmulator : Emulator<M64PlusEmulator>
{
    // Core functions
    using debug_get_mem_ptr_t = void*(CALL*)(m64p_dbg_memptr_type);
    using core_do_cmd_t = m64p_error(CALL*)(m64p_command, int, void*);

    static constexpr std::size_t BSWAP_SIZE{4};


    explicit M64PlusEmulator(shared_object_t core_hdl):
        core_hdl_(core_hdl)
    {
        load_function(core_hdl_, do_cmd_, "CoreDoCommand");
        load_function(core_hdl_, get_mem_ptr_, "DebugMemGetPointer");

        if(!(do_cmd_ && get_mem_ptr_()))
            throw std::runtime_error("Invalid Mupen64Plus core handle");

        base_ptr_ = reinterpret_cast<std::uint8_t*>(get_mem_ptr_(M64P_DBG_PTR_RDRAM));

        if(!base_ptr_)
            throw std::runtime_error("Invalid N64 base pointer");
    }

    /*
     * To implement your own emulator class you need to override this function
     */
    EmulatorState state() final
    {
        int result{};

        if(do_cmd_(M64CMD_CORE_STATE_QUERY, M64CORE_EMU_STATE, &result) != M64ERR_SUCCESS)
            throw std::runtime_error("Failed to query emulator state");

        switch(result)
        {
            case 1:
                return EmulatorState::STOPPED;
            case 2:
                return EmulatorState::RUNNING;
            case 3:
                return EmulatorState::PAUSED;
            default:
                break;
        }

        throw std::runtime_error("Invalid emulator state");
        return EmulatorState::STOPPED;
    }

    // You also have to implement this non virtual function, addr is in physical address space
    bool read_byte(n64_ptr_t addr, std::uint8_t& v)
    {
        v = base_ptr_[convert_addr(addr)];
        return true;
    }

    // Same for this one
    bool write_byte(n64_ptr_t addr, std::uint8_t v)
    {
        base_ptr_[convert_addr(addr)] = v
    }

private:
    static n64_ptr_t convert_addr(n64_ptr_t addr)
    {
        return (addr - (2 * (addr % BSWAP_SIZE)) + (BSWAP_SIZE - 1));
    }

    shared_object_t core_hdl_{};
    debug_get_mem_ptr_t get_mem_ptr_{};
    core_do_cmd_t do_cmd_{};
    std::uint8_t* base_ptr_{};
};

} // LAN64
