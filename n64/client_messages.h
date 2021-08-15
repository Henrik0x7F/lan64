#ifndef LAN64_CLIENT_MESSAGES_H
#define LAN64_CLIENT_MESSAGES_H

#include "lan64_defs.h"
#include "types.h"


/* Message IDs */
enum
{
    LAN64_CLIENT_MSG_ENABLE = 0,
    LAN64_CLIENT_MSG_DISABLE,
    LAN64_CLIENT_MSG_CONNECTED,
    LAN64_CLIENT_MSG_DISCONNECTED,
    LAN64_CLIENT_MSG_PACKET,
    
    LAN64_CLIENT_MSG_COUNT
};

typedef u8 lan64_client_msg_id_t;

typedef struct
{
    u8 x;
}lan64_client_msg_enable_t;

s32 lan64_parse_client_msg_enable(const void* buf, lan64_msg_size_t len, lan64_client_msg_enable_t* msg);

typedef struct
{
    u8 x;
}lan64_client_msg_disable_t;

s32 lan64_parse_client_msg_disable(const void* buf, lan64_msg_size_t len, lan64_client_msg_disable_t* msg);

typedef struct
{
    lan64_addr_t addr;
    char name[LAN64_NAME_LEN];
}lan64_client_msg_connected_t;

s32 lan64_parse_client_msg_connected(const void* buf, lan64_msg_size_t len, lan64_client_msg_connected_t* msg);

typedef struct
{
    lan64_addr_t addr;
}lan64_client_msg_disconnected_t;

s32 lan64_parse_client_msg_disconnected(const void* buf, lan64_msg_size_t len, lan64_client_msg_disconnected_t* msg);

typedef struct
{
    lan64_addr_t addr;
    lan64_msg_size_t len;
    u8 buf[LAN64_MAX_PACKET_LEN];
}lan64_client_msg_packet_t;

s32 lan64_parse_client_msg_packet(const void* buf, lan64_msg_size_t len, lan64_client_msg_packet_t* msg);

#endif
