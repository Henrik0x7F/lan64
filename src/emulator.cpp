//
// Created by henrik on 20.08.21.
//

#include "lan64/emulator.hpp"


namespace LAN64
{

bool IEmulator::check_bounds(n64_size_t addr, n64_size_t len)
{
    return (addr + len <= 0x800000);
}

n64_size_t IEmulator::logical_to_physical(n64_size_t addr)
{
    return (addr - 0x80000000);
}

} // LAN64
