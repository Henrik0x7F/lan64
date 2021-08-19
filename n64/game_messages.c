#include "game_messages.h" 
#include "util.h"


s32 lan64_send_msg_log(lan64_queue_t* queue, const lan64_game_msg_log_t* msg)
{
    u8 buf[LAN64_MAX_MSG_LEN];
    const u32 header_len = sizeof(lan64_game_msg_id_t) + sizeof(lan64_loglevel_t);
    const u32 actual_len = header_len + strnlen(msg->msg, LAN64_MAX_MSG_LEN - header_len - 1) + 1;

    buf[0] = LAN64_GAME_MSG_LOG; /* Message type */
    buf[sizeof(lan64_game_msg_id_t)] = msg->loglevel; /* Loglevel */
    strncpy((buf + header_len), msg->msg, LAN64_MAX_MSG_LEN - header_len); /* Message */

    return lan64_queue_send(queue, buf, actual_len);
}

s32 lan64_send_msg_packet(lan64_queue_t* queue, const lan64_game_msg_packet_t* msg)
{
    u8 buf[LAN64_MAX_MSG_LEN];
    const u32 header_len = sizeof(lan64_game_msg_id_t) + sizeof(lan64_addr_t) + sizeof(lan64_msg_size_t);
    const u32 actual_len = header_len + msg->len;
    if(actual_len > LAN64_MAX_MSG_LEN)
        return 0;

    buf[0] = LAN64_GAME_MSG_PACKET;
    buf[sizeof(lan64_game_msg_id_t)] = msg->receiver;
    buf[sizeof(lan64_game_msg_id_t) + sizeof(lan64_addr_t)] = msg->len;
    memcpy((buf + header_len), msg->data, msg->len);

    return lan64_queue_send(queue, buf, actual_len);
}
