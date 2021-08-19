#ifndef LAN64_GAME_MESSAGES_H
#define LAN64_GAME_MESSAGES_H

#include "lan64_defs.h"
#include "types.h"
#include "message_queue.h"


/* Message IDs */
enum
{
    LAN64_GAME_MSG_LOG = 0,
    LAN64_GAME_MSG_PACKET,
    
    LAN64_GAME_MSG_COUNT
};

#define LAN64_MAX_LOG_LEN (LAN64_MAX_MSG_LEN - sizeof(lan64_game_msg_id_t) - sizeof(lan64_loglevel_t))

typedef struct
{
    lan64_loglevel_t loglevel;
    const char* msg;
}lan64_game_msg_log_t;

lan64_s32 lan64_send_msg_log(lan64_queue_t* queue, const lan64_game_msg_log_t* msg);

typedef struct
{
    lan64_addr_t receiver;
    lan64_msg_size_t len;
    const lan64_u8* data;
}lan64_game_msg_packet_t;

lan64_s32 lan64_send_msg_packet(lan64_queue_t* queue, const lan64_game_msg_packet_t* msg);

#endif
