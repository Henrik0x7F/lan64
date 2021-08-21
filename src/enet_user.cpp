//
// Created by henrik on 21.08.21.
//

#include "lan64/enet_user.hpp"

#include <enet/enet.h>


namespace LAN64
{

ENetUser::ENetUser()
{
    std::lock_guard m(lock_s);

    if(ref_count_s == 0)
    {
        if(enet_initialize() != 0)
            throw std::runtime_error("Failed to initialize ENet");
    }

    ++ref_count_s;
}

ENetUser::~ENetUser()
{
    std::lock_guard m(lock_s);

    if(ref_count_s == 1)
        enet_deinitialize();

    --ref_count_s;
}

} // LAN64
