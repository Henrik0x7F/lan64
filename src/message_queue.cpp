//
// Created by henrik on 22.08.21.
//

#include "lan64/message_queue.hpp"


/*
..xxx....
 ^   ^

empty
.........
^^

full
xxxxxxxxx
    ^
    ^
*/

namespace LAN64
{

MessageQueue::MessageQueue(IEmulator& hdl, n64_ptr_t in, n64_ptr_t out):
    hdl_(&hdl),
    in_state_(in),
    out_state_(out)
{}

std::size_t MessageQueue::poll(void* buf, std::size_t len)
{
    auto msg_size{static_cast<MsgSize>((in_available() > 0) * read_msg_size())};

    if(msg_size == 0 || msg_size > len || buf == nullptr)
        return msg_size;

    // Read message
    QueueSize eptr, size;
    hdl_->read_field(in_state_, &State::extract_ptr, eptr);
    hdl_->read_field(in_state_, &State::size, size);
    read(eptr + sizeof(MsgSize) + 1, buf, static_cast<QueueSize>(len));

    // Advance eptr
    hdl_->write_field(in_state_, &State::extract_ptr, wrap(eptr + msg_size + sizeof(MsgSize), size));

    return msg_size;
}

bool MessageQueue::send(const void* buf, std::size_t len)
{
    if(out_free() < len + sizeof(MsgSize) || len > MAX_MSG_SIZE)
        return false;

    // Write message len
    write_msg_size(static_cast<MsgSize>(len));

    // Write message
    QueueSize iptr, size;
    hdl_->read_field(out_state_, &State::insert_ptr, iptr);
    hdl_->read_field(out_state_, &State::size, size);
    write(iptr + sizeof(MsgSize), buf, static_cast<MsgSize>(len));

    // Advence iptr
    hdl_->write_field(out_state_, &State::insert_ptr, wrap(iptr + static_cast<MsgSize>(len) + sizeof(MsgSize), size));

    return true;
}

std::size_t MessageQueue::out_free() const
{
    QueueSize eptr, iptr, size;

    hdl_->read_field(out_state_, &State::extract_ptr, eptr);
    hdl_->read_field(out_state_, &State::insert_ptr, iptr);
    hdl_->read_field(out_state_, &State::size, size);

    return size - in_use(size, eptr, iptr) - 1;
}

std::size_t MessageQueue::in_available() const
{
    QueueSize eptr, iptr, size;

    hdl_->read_field(in_state_, &State::extract_ptr, eptr);
    hdl_->read_field(in_state_, &State::insert_ptr, iptr);
    hdl_->read_field(in_state_, &State::size, size);

    return static_cast<std::size_t>(in_use(size, eptr, iptr));
}

MessageQueue::MsgSize MessageQueue::read_msg_size() const
{
    QueueSize eptr, size;
    n64_ptr_t buf;

    hdl_->read_field(in_state_, &State::extract_ptr, eptr);
    hdl_->read_field(in_state_, &State::size, size);
    hdl_->read_field(in_state_, &State::buf, buf);

    MsgSize msg_size{};
    hdl_->read(buf + wrap(eptr + 1, size), msg_size);
    return msg_size;
}

void MessageQueue::write_msg_size(MsgSize msg_size)
{
    QueueSize iptr, size;
    n64_ptr_t buf;

    hdl_->read_field(out_state_, &State::insert_ptr, iptr);
    hdl_->read_field(out_state_, &State::size, size);
    hdl_->read_field(out_state_, &State::buf, buf);

    hdl_->write(buf + wrap(iptr, size), msg_size);
}

void MessageQueue::read(QueueSize pos, void* buf, QueueSize len)
{
    QueueSize size;
    n64_ptr_t buf_addr;

    hdl_->read_field(in_state_, &State::size, size);
    hdl_->read_field(in_state_, &State::buf, buf_addr);

    for(std::size_t i{}; i < len; ++i)
    {
        hdl_->read(buf_addr + wrap(pos + i, size), reinterpret_cast<std::uint8_t*>(buf)[i]);
    }
}

void MessageQueue::write(QueueSize pos, const void* buf, QueueSize len)
{
    QueueSize size;
    n64_ptr_t buf_addr;

    hdl_->read_field(out_state_, &State::size, size);
    hdl_->read_field(out_state_, &State::buf, buf_addr);

    for(std::size_t i{}; i < len; ++i)
    {
        hdl_->write(buf_addr + wrap(pos + i, size), reinterpret_cast<const std::uint8_t*>(buf)[i]);
    }
}

MessageQueue::QueueSize MessageQueue::wrap(QueueSize idx, QueueSize size)
{
    return idx % size;
}

MessageQueue::QueueSize MessageQueue::in_use(QueueSize size, QueueSize eptr, QueueSize iptr)
{
    return (eptr != iptr) * (((eptr > iptr) * size) + iptr - eptr - 1) + (eptr == iptr) * (size - 1);
}

} // LAN64
