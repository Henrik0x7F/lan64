#ifndef LAN64_LAN64_H
#define LAN64_LAN64_H

#include "types.h"
#include "lan64_defs.h"


/* Event structs */
typedef struct
{
    u8 x;
}lan64_event_enabled_t;

typedef struct
{
    u8 x;
}lan64_event_disabled_t;

/* Connected event */
typedef struct
{
    lan64_addr_t addr;
    char name[LAN64_NAME_LEN];
}lan64_event_connected_t;

/* Disconnected event */
typedef struct
{
    lan64_addr_t addr;
}lan64_event_disconnected_t;


/* Event union */
typedef struct
{
    u8 type;
    union
    {
        lan64_event_enabled_t enabled;
        lan64_event_disabled_t disabled;
        lan64_event_connected_t connected;
        lan64_event_disconnected_t disconnected;
    };

}lan64_event_t;

/* Event handlers */

/* Called upon receiving an event
 * Event
 * User pointer */
typedef void(*lan64_event_handler_t)(lan64_event_t, void*);
/* Called upon receiving a packet
 * Sender
 * Packet buffer
 * Buffer len
 * User pointer */
typedef void(*lan64_packet_handler_t)(lan64_addr_t, const void*, u32, void*);

/* Handler struct */
typedef struct
{
    lan64_event_handler_t event_handler;
    lan64_packet_handler_t packet_handler;
}lan64_handlers_t;


/* Initialize LAN64, returns 1 on success, otherwise 0 */
s32 lan64_init(const lan64_handlers_t* handlers);

/* Check if LAN64 has been initialized */
s32 lan64_initialized();

/* Update LAN64. LAN64 has to be initialized before, call every frame
 * Parameters are passed to event handlers */
s32 lan64_update(void* packet_usr, void* event_usr);

/* Send a log message */
s32 lan64_log(lan64_loglevel_t loglevel, const char* msg);

/* Send a packet to addr. Use LAN64_BROADCAST to send to everyone. Returns 1 on success */
s32 lan64_send(lan64_addr_t addr, const void* buf, lan64_msg_size_t len);

#endif
