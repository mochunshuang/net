#pragma once

namespace mcs::taps::data
{
    // （有关此类成帧协议的示例，请参阅[RFC9329]）
    // 实现消息帧器的API可能会有所不同，具体取决于实现；实现消息帧器的指南可以在[RFC9623]中找到。
    /*
Figure 1: Protocol Stack Showing a Message Framer
  Initiate()   Send()   Receive()   Close()
      |          |         ^          |
      |          |         |          |
 +----v----------v---------+----------v-----+
 |                Connection                |
 +----+----------+---------^----------+-----+
      |          |         |          |
      |      +-----------------+      |
      |      |    Messages     |      |
      |      +-----------------+      |
      |          |         |          |
 +----v----------v---------+----------v-----+
 |                Framer(s)                 |
 +----+----------+---------^----------+-----+
      |          |         |          |
      |      +-----------------+      |
      |      |   Byte-stream   |      |
      |      +-----------------+      |
      |          |         |          |
 +----v----------v---------+----------v-----+
 |         Transport Protocol Stack         |
 +------------------------------------------+


framer := NewHTTPMessageFramer()
Preconnection.AddFramer(framer)

    */

    // NOTE: 看出来了，应该是模板，耦合协议栈
    struct message_framers
    {
    };

    /*
        messageContext := NewMessageContext()
        messageContext.add(framer, key, value)
        Connection.Send(messageData, messageContext)

//使用：比如协议头，就是 HTTP的协议头
messageContext.get(framer, key) -> value
//例子：
httpFramer := NewHTTPMessageFramer()
...
messageContext := NewMessageContext()
messageContext.add(httpFramer, "accept", "text/html")
    */
    struct framing_metadata
    {
    };

}; // namespace mcs::taps::data