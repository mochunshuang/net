#pragma once

namespace mcs::net::operators
{
    // TAPS是显式基于消息的，async_receive（）是一个定制点，旨在接收整个消息、文件等。在TAPS下构成消息的决定委托给可定制的成帧器，该成帧器在连接建立时提供。
    // TAPS消息操作始终包含调用者提供的消息上下文。[p2762]中定义的async_receive（）方法被修改为接受消息上下文。

    /*
TAPS允许部分接收操作，类似于async_read_some（）。在TAPS方案中，应用程序发出多个部分接收操作，指示在传递部分结果之前最多接收多少个八位字节。指定相同MessageContext实例的所有此类部分接收操作都是相同逻辑接收操作的一部分。这里的建议是扩展async_receive（）的签名以包含最大读取长度值，或者引入async_receive_some（）操作。

对async_receive（）的每次调用都会返回一个接收-发送方，可以通过.
connect（）ed来接收该操作的结果。应用程序可以通过控制任何时刻正在进行的并发async_receive（）操作的数量来限制连接并潜在地提供背压。

async_receive（）最终传递的set_value（）调用包括启动操作时提供的消息上下文，以及（可能是强类型的）消息或部分消息结果。
*/
    // async_receive（）操作可取消通常的stop_token机制与任何其他异步操作下std::执行建议。
}; // namespace mcs::net::operators