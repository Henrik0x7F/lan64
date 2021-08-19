#ifndef LAN64_DEFS_H
#define LAN64_DEFS_H

#include "types.h"


typedef lan64_u8 lan64_loglevel_t;
typedef lan64_u8 lan64_addr_t;
typedef lan64_u16 lan64_queue_size_t;
typedef lan64_u8 lan64_msg_size_t;

/* LAN64 version info */
#define LAN64_VERSION_MAJOR 1
#define LAN64_VERSION_MINOR 0
#define LAN64_VERSION_PROTO 1

#define LAN64_MAX_MSG_LEN 255
#define LAN64_BROADCAST 0
#define LAN64_MAX_CLIENTS 255
#define LAN64_MAX_PACKET_LEN (LAN64_MAX_MSG_LEN - sizeof(lan64_addr_t) - sizeof(lan64_msg_size_t) - 1)
#define LAN64_NAME_LEN 25

/* Message queue sizes */
enum
{
    LAN64_QUEUE_IN_SIZE = 1024,
    LAN64_QUEUE_OUT_SIZE = 1024
};

/* Log levels */
enum
{
    LAN64_LOG_ERROR = 0,
    LAN64_LOG_WARNING,
    LAN64_LOG_INFO,
    LAN64_LOG_DEBUG,

    LAN64_LOG_COUNT
};

/* Events */
enum
{
    LAN64_EVENT_ENABLED = 0,        /* LAN64 support has been enabled by the emulator */
    LAN64_EVENT_DISABLED,           /* LAN64 support has been disabled by the emulator */
    LAN64_EVENT_CONNECTED,          /* The emulator has connected to a virtual LAN network */
    LAN64_EVENT_DISCONNECTED,       /* The emulator has disconnected from a virtual LAN network */
    LAN64_EVENT_OTHER_CONNECTED,    /* Another emulator has connected */
    LAN64_EVENT_OTHER_DISCONNECTED, /* Another emulator has disconnected */

    LAN64_EVENT_COUNT
};

#endif
