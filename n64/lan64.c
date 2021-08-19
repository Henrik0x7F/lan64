#include "lan64.h"

#include "util.h"
#include "game_messages.h"
#include "client_messages.h"
#include "message_queue.h"


/* Location of the pointer to lan64's header */
#define LAN64_HEADER_PTR_ADDR 0x807FFFFC
/* Location of magic number */
#define LAN64_MAGIC_NUMBER_ADDR 0x807FFFF8
/* Magic number indicating to the client that LAN64 finished initializing */
#define LAN64_MAGIC_NUMBER 0x4C414E40 /* "LAN"64 */


/* LAN64 Header */
typedef struct
{
    lan64_u16 version_proto;
    lan64_u8 version_major;
    lan64_u8 version_minor;
    lan64_queue_t* in_queue,
                 * out_queue;
}lan64_header_t;

/* LAN64 global state */
typedef struct
{
    /* Message queues */
    lan64_queue_t in_queue,
                  out_queue;
    lan64_u8 in_queue_buf[LAN64_QUEUE_IN_SIZE],
       out_queue_buf[LAN64_QUEUE_OUT_SIZE];
    lan64_handlers_t event_handlers;
    lan64_addr_t addr;
}lan64_state_t;

/* Header */
static lan64_header_t lan64_header_l;

/* Global state */
lan64_state_t lan64_state_g;

#define LAN64_HEADER_PTR (*((lan64_header_t**)LAN64_HEADER_PTR_ADDR))
#define LAN64_MAGIC_NUMBER_PTR ((lan64_u32*)LAN64_MAGIC_NUMBER_ADDR)


static void dummy_event_handler(lan64_event_t a, void* b)
{
    (void)a;
    (void)b;
}

static void dummy_packet_handler(lan64_addr_t a, const void* b, lan64_u32 c, void* d)
{
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}

lan64_s32 lan64_initialize(const lan64_handlers_t* handlers)
{
    lan64_memset(&lan64_state_g, 0, sizeof(lan64_state_t));

    /* Point header pointer to header */
    LAN64_HEADER_PTR = &lan64_header_l;

    /* LAN64 version info */
    lan64_header_l.version_proto = LAN64_VERSION_PROTO;
    lan64_header_l.version_major = LAN64_VERSION_MAJOR;
    lan64_header_l.version_minor = LAN64_VERSION_MINOR;

    /* Init message queues */
    lan64_queue_create(&lan64_state_g.in_queue, lan64_state_g.in_queue_buf, LAN64_QUEUE_IN_SIZE);
    lan64_queue_create(&lan64_state_g.out_queue, lan64_state_g.out_queue_buf, LAN64_QUEUE_OUT_SIZE);

    lan64_header_l.out_queue = &lan64_state_g.out_queue;
    lan64_header_l.in_queue = &lan64_state_g.in_queue;

    /* Init callbacks */
    lan64_state_g.event_handlers.event_handler = (handlers && handlers->event_handler) ? handlers->event_handler : &dummy_event_handler;
    lan64_state_g.event_handlers.packet_handler = (handlers && handlers->packet_handler) ? handlers->packet_handler : &dummy_packet_handler;

    /* Local address placeholder */
    lan64_state_g.addr = LAN64_BROADCAST;

    /* Initialization complete, set magic number */
    *LAN64_MAGIC_NUMBER_PTR = LAN64_MAGIC_NUMBER;

    return 1;
}

lan64_s32 lan64_is_initialized()
{
    return (*LAN64_MAGIC_NUMBER_PTR == LAN64_MAGIC_NUMBER);
}

lan64_s32 lan64_update(void* packet_usr, void* event_usr)
{
    lan64_msg_size_t msg_size = 0;
    lan64_u8 msg_buf[LAN64_MAX_MSG_LEN];

    /* Handle incoming messages */
    while((msg_size = lan64_queue_poll(&lan64_state_g.in_queue, msg_buf, LAN64_MAX_MSG_LEN)) != 0)
    {
        switch(msg_buf[0])
        {
            case LAN64_CLIENT_MSG_ENABLE:
            {
                lan64_client_msg_enable_t msg;
                lan64_event_t event;
                lan64_parse_client_msg_enable(msg_buf, msg_size, &msg);
                event.type = LAN64_EVENT_ENABLED;
                lan64_state_g.event_handlers.event_handler(event, event_usr);
                break;
            }
            case LAN64_CLIENT_MSG_DISABLE:
            {
                lan64_client_msg_disable_t msg;
                lan64_event_t event;
                lan64_parse_client_msg_disable(msg_buf, msg_size, &msg);
                event.type = LAN64_EVENT_DISABLED;
                lan64_state_g.event_handlers.event_handler(event, event_usr);
                break;
            }
            case LAN64_CLIENT_MSG_CONNECTED:
            {
                lan64_client_msg_connected_t msg;
                lan64_event_t event;
                if(!lan64_parse_client_msg_connected(msg_buf, msg_size, &msg))
                    break;
                if(lan64_state_g.addr == LAN64_BROADCAST)
                    event.type = LAN64_EVENT_CONNECTED;
                else
                    event.type = LAN64_EVENT_OTHER_CONNECTED;
                event.connected.addr = msg.addr;
                lan64_memcpy(event.connected.name, msg.name, LAN64_NAME_LEN);
                lan64_state_g.event_handlers.event_handler(event, event_usr);
                break;
            }
            case LAN64_CLIENT_MSG_DISCONNECTED:
            {
                lan64_client_msg_disconnected_t msg;
                lan64_event_t event;
                if(!lan64_parse_client_msg_disconnected(msg_buf, msg_size, &msg))
                    break;
                if(lan64_state_g.addr == msg.addr)
                {
                    event.type = LAN64_EVENT_DISCONNECTED;
                    lan64_state_g.addr = LAN64_BROADCAST;
                }
                else
                {
                    event.type = LAN64_EVENT_OTHER_DISCONNECTED;
                }
                event.disconnected.addr = msg.addr;
                lan64_state_g.event_handlers.event_handler(event, event_usr);
                break;
            }
            case LAN64_CLIENT_MSG_PACKET:
            {
                lan64_client_msg_packet_t msg;
                if(!lan64_parse_client_msg_packet(msg_buf, msg_size, &msg))
                    break;

                lan64_state_g.event_handlers.packet_handler(msg.addr, msg.buf, msg.len, packet_usr);
                break;
            }
        }
    }

    return 1;
}

lan64_s32 lan64_log(lan64_loglevel_t loglevel, const char* log_message)
{
    lan64_game_msg_log_t msg;

    msg.loglevel = loglevel;
    msg.msg = log_message;

    return lan64_send_msg_log(&lan64_state_g.out_queue, &msg);
}

lan64_s32 lan64_send(lan64_addr_t addr, const void* buf, lan64_msg_size_t len)
{
    lan64_game_msg_packet_t msg;
    msg.data = (const lan64_u8*)buf;
    msg.receiver = addr;
    msg.len = len;

    return lan64_send_msg_packet(&lan64_state_g.out_queue, &msg);
}
