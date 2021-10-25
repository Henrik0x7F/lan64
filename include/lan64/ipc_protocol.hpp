//
// Created by Henrik on 25.10.2021.
//

#pragma once

#include <vector>
#include "net_protocol.hpp"


namespace LAN64
{

using IPC_MsgSize = std::uint8_t;

enum struct IPC_ClientMsg : std::uint8_t
{
    ENABLE = 0,
    DISABLE,
    CONNECTED,
    DISCONNECTED,
    PACKET,

    COUNT
};

enum struct IPC_GameMsg : std::uint8_t
{
    LOG = 0,
    PACKET,

    COUNT
};

struct IPC_Message
{
    virtual std::size_t serialized_size() const = 0;
    virtual bool serialize(void* buf, std::size_t len) const = 0;
};

struct GameMessageLog
{
    enum struct Level
    {
        ERROR = 0,
        WARNING,
        INFO,
        DEBUG,

        COUNT
    };

    bool parse(const void* buf, std::size_t len);

    std::string logmessage;
    Level loglevel;
};

struct GameMessagePacket
{
    bool parse(const void* buf, std::size_t len);

    Lan64Addr receiver;
    std::vector<std::uint8_t> data;
};

struct ClientMessageEnable : IPC_Message
{
    std::size_t serialized_size() const override
    {
        return sizeof(IPC_ClientMsg::ENABLE);
    }

    bool serialize(void* buf, std::size_t len) const override
    {
        if(len < sizeof(IPC_ClientMsg::ENABLE))
            return false;

        reinterpret_cast<IPC_ClientMsg*>(buf)[0] = IPC_ClientMsg::ENABLE;
        return true;
    }
};

struct ClientMessageDisable : IPC_Message
{
    std::size_t serialized_size() const override
    {
        return sizeof(IPC_ClientMsg);
    }

    bool serialize(void* buf, std::size_t len) const override
    {
        if(len < sizeof(IPC_ClientMsg))
            return false;

        reinterpret_cast<IPC_ClientMsg*>(buf)[0] = IPC_ClientMsg::DISABLE;
        return true;
    }
};

struct ClientMessageConnected : IPC_Message
{
    std::size_t serialized_size() const override;
    bool serialize(void* buf, std::size_t len) const override;

    Lan64Addr addr;
    std::string name;
};

struct ClientMessageDisconnected : IPC_Message
{
    std::size_t serialized_size() const override;
    bool serialize(void* buf, std::size_t len) const override;

    Lan64Addr addr;
};

struct ClientMessagePacket : IPC_Message
{
    std::size_t serialized_size() const override;
    bool serialize(void* buf, std::size_t len) const override;

    Lan64Addr addr;
    std::vector<std::uint8_t> data;
};

}
