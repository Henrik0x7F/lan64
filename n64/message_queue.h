#ifndef LAN64_MESSAGE_QUEUE_H
#define LAN64_MESSAGE_QUEUE_H

#include "lan64_defs.h"
#include "util.h"
#include "types.h"


typedef struct
{
    lan64_queue_size_t size,
                       iptr,
                       eptr;
    u8* message_array;
}lan64_queue_t;


/* Initialize a message queue
 * It's the caller's responisbility to allocate and free memory
 */
void lan64_queue_create(lan64_queue_t* queue, void* message_array, lan64_queue_size_t queue_size);

/* Send a message
 * Returns 1 if successful
 */
s32 lan64_queue_send(lan64_queue_t* queue, const void* msg, lan64_msg_size_t msg_size);

/* Polls for incoming messages
 * Returns len of message, 0 if none available
 */
lan64_msg_size_t lan64_queue_poll(lan64_queue_t* queue, void* buf, lan64_msg_size_t len);

/* For profiling. Does not account for protocol overhead */
void lan64_queue_info(const lan64_queue_t* queue, lan64_queue_size_t* queue_size, lan64_queue_size_t* in_use, lan64_queue_size_t* free);

/* Returns how many bytes are available to write */
lan64_queue_size_t lan64_queue_out_free(const lan64_queue_t* queue);

/* Returns how many bytes are available to read */
lan64_queue_size_t lan64_queue_in_available(const lan64_queue_t* queue);

#endif
