//
// Created by henrik on 22.08.21.
//

#pragma once

#include <limits>
#include "lan64/emulator.hpp"


namespace LAN64
{

struct MessageQueue
{
    using QueueSize = std::uint16_t;
    using MsgSize = std::uint8_t;

    static constexpr std::size_t MAX_MSG_SIZE{std::numeric_limits<MsgSize>::max()};

    struct State
    {
        QueueSize size,
        insert_ptr,
        extract_ptr;
        n64_ptr_t buf; // u8
    };

    MessageQueue(IEmulator& hdl, n64_ptr_t in, n64_ptr_t out);

    std::size_t poll(void* buf, std::size_t len);

    bool send(const void* buf, std::size_t len);

    std::size_t out_free();

    std::size_t in_available();

private:
    MsgSize read_msg_size();
    void write_msg_size(MsgSize size);
    void read(QueueSize pos, void* buf, QueueSize len);
    void write(QueueSize pos, const void* buf, QueueSize len);

    static QueueSize wrap(QueueSize idx, QueueSize size);
    static QueueSize in_use(QueueSize size, QueueSize eptr, QueueSize iptr);

    IEmulator* hdl_{};
    n64_ptr_t in_state_{},
              out_state_{};
};

} // LAN64

