//
// Created by henrik on 21.08.21.
//

#pragma once

#include <mutex>


namespace LAN64
{

struct ENetUser
{
protected:
    ENetUser();
    ~ENetUser();

private:
    static std::size_t ref_count_s;
    static std::mutex lock_s;
};

} // LAN64
