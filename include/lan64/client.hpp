//
// Created by henrik on 21.08.21.
//

#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <vector>
#include "emulator.hpp"
#include "enet_host.hpp"


namespace LAN64
{

struct Client : ENetClient::Listener
{
    struct Listener
    {
        virtual void on_connect(bool success) = 0;
        virtual void on_disconnect() = 0;
        virtual void on_player_connected(std::string name, Lan64Addr addr) = 0;
    };

    Client() = default;

    Client(const Client& other);

    bool hook(IEmulator& emu);
    void unhook();

    void connect(const std::string& hostname, std::uint16_t port);
    void disconnect();
    void disconnect_now();

    bool connected() const;
    std::size_t player_count() const;

private:
    ENetClient client_(*this);
    IEmulator* emu_{};
    std::thread thread_;
    std::atomic_bool running_{};
};

} // LAN64
