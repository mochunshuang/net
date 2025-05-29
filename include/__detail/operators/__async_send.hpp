#pragma once

namespace mcs::net::operators
{
    // TAPS是显式消息库，async_send（）是旨在发送整个消息、文件等的自定义点。
    // 关于如何将消息编码到TAPS下的底层传输的决定委托给可定制的帧器，该帧器在连接建立时提供。

    // async_send（）的返回类型是一个send-sender。除了在发送消息的过程中可能出现的任何错误之外，该发件人还可以传递一些值，这些值指示消息已经发送，或者可能在消息发送之前消息发送超时

    // TAPS允许部分发送操作。这种部分发送发送单个逻辑消息的部分，通过传递相同的MessageContext实例将其关联为单个逻辑事务。
    //
}; // namespace mcs::net::operators