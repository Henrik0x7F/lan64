#include "client_messages.h"
#include "util.h"


lan64_s32 lan64_parse_client_msg_enable(const void* buf, lan64_msg_size_t len, lan64_client_msg_enable_t* msg)
{
    (void)buf;
    (void)len;
    (void)msg;

    return 1;
}

lan64_s32 lan64_parse_client_msg_disable(const void* buf, lan64_msg_size_t len, lan64_client_msg_disable_t* msg)
{
    (void)buf;
    (void)len;
    (void)msg;

    return 1;
}

lan64_s32 lan64_parse_client_msg_connected(const void* buf, lan64_msg_size_t len, lan64_client_msg_connected_t* msg)
{
    const lan64_u8* arr = (const lan64_u8*)buf;

    if(arr[0] != LAN64_CLIENT_MSG_CONNECTED)
        return 0;

    if(len != (sizeof(lan64_addr_t) + LAN64_NAME_LEN + sizeof(lan64_msg_id_t)))
        return 0;

    msg->addr = arr[sizeof(lan64_msg_id_t)];
    lan64_memcpy(msg->name, arr + sizeof(lan64_addr_t) + sizeof(lan64_msg_id_t), LAN64_NAME_LEN);

    return 1;
}

lan64_s32 lan64_parse_client_msg_disconnected(const void* buf, lan64_msg_size_t len, lan64_client_msg_disconnected_t* msg)
{
    const lan64_u8* arr = (const lan64_u8*)buf;

    if(arr[0] != LAN64_CLIENT_MSG_DISCONNECTED)
        return 0;

    if(len != (sizeof(lan64_msg_id_t) + sizeof(lan64_addr_t)))
        return 0;

    msg->addr = arr[sizeof(lan64_msg_id_t)];

    return 1;
}

lan64_s32 lan64_parse_client_msg_packet(const void* buf, lan64_msg_size_t len, lan64_client_msg_packet_t* msg)
{
    const lan64_u8* arr = (const lan64_u8*)buf;

    if(arr[0] != LAN64_CLIENT_MSG_PACKET)
        return 0;

    msg->addr = arr[sizeof(lan64_msg_id_t)];
    msg->len = arr[sizeof(lan64_msg_id_t) + sizeof(lan64_addr_t)];

    if(msg->len > LAN64_MAX_PACKET_LEN)
        return 0;

    if(len != (sizeof(lan64_msg_id_t) + sizeof(lan64_addr_t) + sizeof(lan64_msg_size_t) + msg->len))
        return 0;

    lan64_memcpy(msg->buf, arr + (sizeof(lan64_msg_id_t) + sizeof(msg->addr) + sizeof(msg->len)), msg->len);

    return 1;
}
