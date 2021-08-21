//
// Created by henrik on 21.08.21.
//

#pragma once

#include <chrono>
#include <unordered_map>
#include <enet/enet.h>
#include "lan64/enet_user.hpp"
#include "lan64/resource_handle.hpp"


namespace LAN64
{

using HostHandle = ResourceHandle<&enet_host_destroy>;
using PeerHandle = ResourceHandle<&enet_peer_reset>;
using PacketHandle = ResourceHandle<&enet_packet_destroy>;


struct ENetServer : ENetUser
{
    struct Listener
    {
        virtual void on_server_connect(ENetPeer& peer, std::uint32_t data) = 0;
        virtual void on_server_disconnect(ENetPeer& peer, std::uint32_t data) = 0;
        virtual void on_server_packet(ENetPeer& peer, const void* buf, std::size_t len, std::uint8_t channel) = 0;
    };

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    explicit ENetServer(Listener& listener);

    void set_listener(Listener& listener);

    void start(std::uint16_t port, std::size_t slots, std::size_t channels, std::uint32_t in_bw = 0, std::uint32_t out_bw = 0);

    void stop();

    void disconnect_client(ENetPeer& peer, std::uint32_t timeout_ms, std::uint32_t data = 0);

    void disconnect_all_clients(std::uint32_t timeout_ms, std::uint32_t data = 0);

    void send(ENetPeer& peer, const void* buf, std::size_t len, std::uint8_t channel, std::uint32_t flags = 0);

    void broadcast(const void* buf, std::size_t len, std::uint8_t channel, std::uint32_t flags = 0);

    void update();

    std::size_t client_count() const;

private:
    struct ClientInfo
    {
        bool is_disconnecting{};
        std::uint32_t disconnect_timeout{};
        TimePoint disconnect_start{};
    };

    Listener* listener_{};
    HostHandle host_;
    std::unordered_map<ENetPeer*, ClientInfo> clients_;
    std::size_t disconnecting_peers_{};
};

struct ENetClient : ENetUser
{
    struct Listener
    {
        virtual void on_client_connect(bool success, std::uint32_t data) = 0;
        virtual void on_client_disconnect(std::uint32_t data) = 0;
        virtual void on_client_packet(const void* buf, std::size_t len, std::uint8_t channel) = 0;
    };

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    ENetClient(Listener& listener, std::size_t channels, std::uint32_t in_bw = 0, std::uint32_t out_bw = 0);

    void set_listener(Listener& listener);

    void connect(const std::string& hostname, std::uint16_t port, std::uint32_t timeout_ms, std::uint32_t data = 0);
    void disconnect(std::uint32_t timeout_ms, std::uint32_t data = 0);
    void disconnect_now();

    void send(const void* buf, std::size_t len, std::uint8_t channel, std::uint32_t flags = 0);

    void update();

private:
    Listener* listener_{};
    std::size_t channel_count_{};
    HostHandle host_;
    PeerHandle peer_;
    bool connected_{};
    bool disconnecting_{};
    std::uint32_t disconnect_timeout_{},
                  connect_timout_{};
    TimePoint action_time_;
};

} // LAN64
