#pragma once

namespace mcs::net::operators
{
    /**
     连接协商和建立、成帧、消息发送和接收都是TAPS中固有的异步操作。
这些功能的C++设计应使用针对C++26的发送者/接收者词汇表，但该提议不足以涵盖网络用例，需要进一步完善提议的sequence_sender概念。

未来的论文修订将更全面地涵盖对序列发送者的讨论。sequence_sender的概念通过添加一个set_next_value（）操作来扩展发送者，
该操作本身返回一个发送者，该发送者可能会在io_context接收到一个值时被安排稍后执行。
     */
};