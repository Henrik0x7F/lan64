//
// Created by Henrik on 25.10.2021.
//


#include "lan64/ipc_protocol.hpp"


namespace LAN64
{


std::size_t LAN64::ClientMessagePacket::serialized_size() const
{
    return sizeof(IPC_ClientMsg) + (Lan64Addr) + sizeof(IPC_MsgSize) + data.size();
}

bool LAN64::ClientMessagePacket::serialize(void* buf, std::size_t len) const
{
    if(len < serialized_size())
        return false;

    auto arr{reinterpret_cast<std::uint8_t*>(buf)};

    arr[0] = static_cast<std::uint8_t>(IPC_ClientMsg::PACKET);
    arr[1] = addr;
    arr[2] = static_cast<std::uint8_t>(data.size());
    std::copy_n(data.begin(), data.size(), arr + 3);

    return true;
}

std::size_t ClientMessageDisconnected::serialized_size() const
{
    return sizeof(IPC_ClientMsg) + sizeof(Lan64Addr);
}

bool ClientMessageDisconnected::serialize(void* buf, std::size_t len) const
{
    if(len < serialized_size())
        return false;

    auto arr{reinterpret_cast<std::uint8_t*>(buf)};

    arr[0] = static_cast<std::uint8_t>(IPC_ClientMsg::DISCONNECTED);
    arr[1] = addr;

    return true;
}

std::size_t ClientMessageConnected::serialized_size() const
{
    return sizeof(IPC_ClientMsg) + sizeof(Lan64Addr);
}

bool ClientMessageConnected::serialize(void* buf, std::size_t len) const
{
    if(len < serialized_size())
        return false;

    auto arr{reinterpret_cast<std::uint8_t*>(buf)};

    arr[0] = static_cast<std::uint8_t>(IPC_ClientMsg::CONNECTED);
    arr[1] = addr;

    return true;
}

bool GameMessageLog::parse(const void* buf, std::size_t len)
{
    auto arr{reinterpret_cast<const std::uint8_t*>(buf)};

    if(len < 2)
        return false;

    if(static_cast<IPC_GameMsg>(arr[0]) != IPC_GameMsg::LOG)
        return false;

    if(static_cast<GameMessageLog::Level>(arr[1]) >= GameMessageLog::Level::COUNT)
        return false;

    logmessage.assign(MAX_LOG_LEN, '\0');
    std::copy(arr + 2, arr + len, logmessage.data());

    return true;
}

bool GameMessagePacket::parse(const void* buf, std::size_t len)
{
    auto arr{reinterpret_cast<const std::uint8_t*>(buf)};

    if(len < 3)
        return false;

    if(static_cast<IPC_GameMsg>(arr[0]) != IPC_GameMsg::PACKET)
        return false;

    receiver = arr[1];

    // @todo len argument not necessary
    data.resize(len - 3);

    std::copy(arr + 3, arr + len, data.data());

    return true;
}
}
