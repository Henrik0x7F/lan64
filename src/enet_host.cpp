//
// Created by henrik on 21.08.21.
//

#include "lan64/enet_host.hpp"


namespace LAN64
{

static bool valid_ip4(const char* str)
{
    std::uint32_t x{};
    return (inet_pton(AF_INET, str, &x) == 1);
}

ENetClient::ENetClient(ENetClient::Listener& listener, std::size_t channels, std::uint32_t in_bw, std::uint32_t out_bw):
    listener_(&listener),
    channel_count_(channels),
    host_(enet_host_create(nullptr, 1, channels, in_bw, out_bw))
{
    if(!host_)
        throw std::runtime_error("Failed to create ENet client");
}

void ENetClient::set_listener(ENetClient::Listener& listener)
{
    listener_ = &listener;
}

void ENetClient::connect(const std::string& hostname, std::uint16_t port, std::uint32_t timeout_ms, std::uint32_t data)
{
    if(peer_)
        disconnect_now();

    connect_timout_ = timeout_ms;

    ENetAddress addr;
    addr.port = port;

    if(valid_ip4(hostname.c_str()))
        enet_address_set_host_ip(&addr, hostname.c_str());
    else
        enet_address_set_host(&addr, hostname.c_str());

    peer_.reset(enet_host_connect(host_.get(), &addr, channel_count_, data));
    if(!peer_)
    {
        listener_->on_client_connect(false, 0);
        disconnect_now();
    }

    action_time_ = Clock::now();
}

void ENetClient::disconnect(std::uint32_t timeout_ms, std::uint32_t data)
{
    if(!peer_)
    {
        listener_->on_client_disconnect(0);
        return;
    }

    if(!connected_)
    {
        disconnect_now();
        listener_->on_client_connect(false, 0);
        listener_->on_client_disconnect(0);
        return;
    }

    enet_peer_disconnect(peer_.get(), data);

    disconnecting_ = true;
    action_time_ = Clock::now();
}

void ENetClient::ENetClient::disconnect_now()
{
    peer_.reset();
    connected_ = false;
    disconnecting_ = false;
}

void ENetClient::send(const void* buf, std::size_t len, std::uint8_t channel, std::uint32_t flags)
{
    if(!peer_ || !connected_)
        return;

    PacketHandle packet(enet_packet_create(buf, len, flags));

    enet_peer_send(peer_.get(), channel, packet.release());
}

void ENetClient::update()
{
    if(!peer_)
        return;

    if(!connected_)
    {
        if(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - action_time_).count() > connect_timout_)
        {
            disconnect_now();
            listener_->on_client_connect(false, 0);
        }
    }

    if(disconnecting_)
    {
        if(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - action_time_).count() > disconnect_timeout_)
        {
            disconnect_now();
            listener_->on_client_disconnect(0);
        }
    }

    for(ENetEvent event; enet_host_service(host_.get(), &event, 0) > 0;)
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                connected_ = true;
                listener_->on_client_connect(true, event.data);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                disconnect_now();
                listener_->on_client_disconnect(event.data);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
            {
                PacketHandle packet(event.packet);
                listener_->on_client_packet(packet->data, packet->dataLength, event.channelID);
                break;
            }
            default:
                break;
        }
    }
}

ENetServer::ENetServer(ENetServer::Listener& listener):
    listener_(&listener)
{

}

void ENetServer::set_listener(ENetServer::Listener& listener)
{
    listener_ = &listener;
}

void ENetServer::start(std::uint16_t port,
                       std::size_t slots,
                       std::size_t channels,
                       std::uint32_t in_bw,
                       std::uint32_t out_bw)
{
    if(host_)
        stop();

    ENetAddress addr{};
    addr.port = port;
    addr.host = ENET_HOST_ANY;

    host_.reset(enet_host_create(&addr, slots, channels, in_bw, out_bw));
    if(!host_)
        throw std::runtime_error("Failed to create ENet host");
}

void ENetServer::stop()
{
    host_.reset();
}

void ENetServer::disconnect_client(ENetPeer& peer, std::uint32_t timeout_ms, std::uint32_t data)
{
    enet_peer_disconnect(&peer, data);
    auto& client {clients_[&peer]};
    client.disconnect_start = Clock::now();
    client.is_disconnecting = true;
    client.disconnect_timeout = timeout_ms;
    ++disconnecting_peers_;
}

void ENetServer::disconnect_all_clients(std::uint32_t timeout_ms, std::uint32_t data)
{
    for(auto& client : clients_)
    {
        disconnect_client(*client.first, timeout_ms, data);
    }
}

void ENetServer::update()
{
    if(!host_)
        return;

    for(auto iter{clients_.begin()}; (disconnecting_peers_ > 0) && (iter != clients_.end());)
    {
        if(iter->second.is_disconnecting)
        {
            if(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - iter->second.disconnect_start).count() > iter->second.disconnect_timeout)
            {
                enet_peer_disconnect_now(iter->first, 0);
                iter = clients_.erase(iter);
                --disconnecting_peers_;
                continue;
            }
        }

        ++iter;
    }

    for(ENetEvent event{}; enet_host_service(host_.get(), &event, 0) > 0;)
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                clients_[event.peer] = {};
                listener_->on_server_connect(*event.peer, event.data);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                listener_->on_server_disconnect(*event.peer, event.data);
                if(clients_[event.peer].is_disconnecting)
                    --disconnecting_peers_;
                clients_.erase(event.peer);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
            {
                PacketHandle packet(event.packet);
                listener_->on_server_packet(*event.peer, packet->data, packet->dataLength, event.channelID);
                break;
            }
            default:
                break;
        }
    }
}

std::size_t ENetServer::client_count() const
{
    return clients_.size();
}

void ENetServer::send(ENetPeer& peer, const void* buf, std::size_t len, std::uint8_t channel, std::uint32_t flags)
{
    if(!host_)
        return;

    PacketHandle packet(enet_packet_create(buf, len, flags));

    enet_peer_send(&peer, channel, packet.release());
}

void ENetServer::broadcast(const void* buf, std::size_t len, std::uint8_t channel, std::uint32_t flags)
{
    if(!host_)
        return;

    PacketHandle packet(enet_packet_create(buf, len, flags));

    enet_host_broadcast(host_.get(), channel, packet.release());
}

} // LAN64
