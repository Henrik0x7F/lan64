#include "message_queue.h"

#include "util.h"


/* Calculate how many bytes are in use */
static lan64_queue_size_t in_use(const lan64_queue_t* queue)
{
    return (queue->eptr != queue->iptr) * (((queue->eptr > queue->iptr) * queue->size) +
           queue->iptr - queue->eptr - 1) + (queue->eptr == queue->iptr) * (queue->size - 1);
}

static lan64_queue_size_t wrap(lan64_queue_size_t ptr, lan64_queue_size_t size)
{
    return ptr % size;
}

static lan64_msg_size_t read_msg_size(const lan64_queue_t* queue)
{
    return queue->message_array[wrap(queue->eptr + 1, queue->size)];
}

static void write_msg_size(lan64_queue_t* queue, lan64_msg_size_t size)
{
    queue->message_array[wrap(queue->iptr, queue->size)] = size;
}

static void write(lan64_queue_t* queue, lan64_queue_size_t pos, const void* buf, lan64_queue_size_t size)
{
    lan64_queue_size_t i = 0;
    for(; i < size; ++i)
    {
        queue->message_array[wrap(pos + i, queue->size)] = ((const u8*)buf)[i];
    }
}

static void read(const lan64_queue_t* queue, lan64_queue_size_t pos, void* out_buf, lan64_queue_size_t size)
{
    lan64_queue_size_t i = 0;
    for(; i < size; ++i)
    {
        ((u8*)out_buf)[i] = queue->message_array[wrap(pos + i, queue->size)];
    }
}

static lan64_msg_size_t next_msg_size(const lan64_queue_t* queue)
{
    return (lan64_queue_in_available(queue) > 0) * read_msg_size(queue);
}


void lan64_queue_create(lan64_queue_t* queue, void* message_array, lan64_queue_size_t queue_size)
{
    queue->size = queue_size;
    queue->eptr = 0;
    queue->iptr = 1;
    queue->message_array = (u8*)message_array;
}

s32 lan64_queue_send(lan64_queue_t* queue, const void* msg, lan64_msg_size_t msg_size)
{
    /* Check if message size is 0 */
    if(msg_size == 0)
        return 0;

    /* Check if the message fits into the queue */
    if(lan64_queue_out_free(queue) < msg_size + sizeof(lan64_msg_size_t))
        return 0;

    /* Write message size */
    write_msg_size(queue, msg_size);

    /* Write message */
    write(queue, queue->iptr + sizeof(lan64_msg_size_t), msg, msg_size);

    /* Advance insertion ptr */
    queue->iptr = wrap(queue->iptr + msg_size + sizeof(lan64_msg_size_t), queue->size);

    return 1;
}

lan64_msg_size_t lan64_queue_poll(lan64_queue_t* queue, void* buf, lan64_msg_size_t len)
{
    lan64_msg_size_t msg_size = next_msg_size(queue);

    if(msg_size == 0 || msg_size > len || buf == 0)
        return msg_size;

    /* Read message */
    read(queue, queue->eptr + 1 + sizeof(lan64_msg_size_t), buf, msg_size);

    /* Advance extraction ptr */
    queue->eptr = wrap(queue->eptr + msg_size + sizeof(lan64_msg_size_t), queue->size);

    return msg_size;
}

void lan64_queue_info(const lan64_queue_t* queue, lan64_queue_size_t* queue_size, lan64_queue_size_t* is_in_use, lan64_queue_size_t* free)
{
    if(queue_size)
        *queue_size = queue->size;
    if(is_in_use)
        *is_in_use = in_use(queue);
    if(free)
        *free = queue->size - in_use(queue);
}

lan64_queue_size_t lan64_queue_out_free(const lan64_queue_t* queue)
{
    return queue->size - 1 - in_use(queue) - sizeof(lan64_msg_size_t);
}

lan64_queue_size_t lan64_queue_in_available(const lan64_queue_t* queue)
{
    return in_use(queue);
}
