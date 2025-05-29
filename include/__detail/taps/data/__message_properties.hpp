#pragma once

#include <cstdint>
#include <limits>
#include <string_view>

#include "../__detail/__types.hpp"

namespace mcs::taps::data
{
    /**
     * @brief MessageContext对象包含要发送或接收的消息的元数据。
        messageData := "hello"
        messageContext := NewMessageContext()
        messageContext.add(parameter, value)
        Connection.Send(messageData, messageContext)
     */
    // The Message Properties in the following subsections are supported.

    struct message_properties
    {
        // https://www.rfc-editor.org/rfc/rfc9622.html#name-lifetime
        /*
            Name: msgLifetime
            Type:  Numeric (positive)
            Default: Infinite
        */
        struct lifetime
        {
            static constexpr std::string_view name = "msgLifetime"; // NOLINT
            using type = std::uint64_t;
            type value = std::numeric_limits<type>::max();
        };
        /**
         * @brief https://www.rfc-editor.org/rfc/rfc9622.html#name-priority
         * 此属性指定消息相对于通过同一连接发送的其他消息的优先级。数字较低的值表示较高的优先级。
优先级为2的消息将产生优先级为1的消息，优先级为0的消息，依此类推。优先级只能用作发送方调度构造，也可以用于为支持优先级的协议栈指定线路上的优先级。
请注意，此属性不是每个消息的连接优先级覆盖；请参阅第8.1.2节。优先级属性可能会交互，但它们可以独立使用并由不同的机制实现；请参阅第9.2.6节。
        priority:
                Name: msgPriority
                Type: Integer (non-negative)
                Default: 100
         */
        struct priority
        {
            static constexpr std::string_view name = "msgPriority"; // NOLINT
            using type = std::uint32_t;
            type value = 100; // NOLINT
        };

        /**
         * @brief
            Name: msgOrdered
            Type: Boolean
            Default: the queried Boolean value of the Selection Property preserveOrder
         (Section 6.2.4)
         */
        struct ordered
        {
            static constexpr std::string_view name = "msgOrdered"; // NOLINT
            using type = __detail::Boolean;
            type value = false;
        };

        /**
         * @brief
        如果为真，则safelyReplayable指定消息可以安全地多次发送到远程端点进行单个发送操作。
它将数据标记为对某些0-RTT建立技术安全，其中0-RTT数据的重传可能导致远程应用程序多次接收消息。
对于不能防止重复消息的协议，例如UDP，所有消息都需要通过启用此属性标记为“安全可重放”。
要启用协议选择以选择此类协议，需要将safelyReplayable添加到传递给预连接的TransportProperties中。
如果选择了此类协议，则在单个消息上禁用safelyReplayable必须导致SendError。
         */
        struct safely_replayable
        {
            static constexpr std::string_view name = "safelyReplayable"; // NOLINT
            using type = __detail::Boolean;
            type value = false;
        };

        /**
         * @brief
如果为真，则表示消息是应用程序将在连接上发送的最后一条消息。这允许底层协议向远程端点指示连接已在发送方向上有效关闭。
例如，一旦标记为Final的消息已完全发送（标记为endOfMessage表示），基于TCP的连接就可以发送FIN。不支持向给定方向发出连接结束信号的协议将忽略此属性。
最终消息必须始终排序到消息列表的末尾。最终属性会覆盖connectPriity、msgPriity和任何其他会重新排序消息的属性。
如果在连接上已发送标记为最终消息的消息后发送另一条消息，则新消息的发送操作将导致SendError事件。
         *
         */
        struct Final
        {
            static constexpr std::string_view name = "final"; // NOLINT
            using type = __detail::Boolean;
            type value = false;
        };

        /**
         如果此属性是整数，它指定已发送消息的部分的最小长度，从字节0开始，应用程序要求在不因较低层错误而损坏的情况下交付该部分。它用于指定通过校验和进行简单完整性保护的选项。值为0表示不需要计算校验和，枚举值Full
         Coverage表示需要通过校验和保护整个消息。只有Full
         Coverage得到保证：任何其他请求都是咨询性的，这可能导致应用Full Coverage。
         */
        struct SendingCorruptionProtectionLength
        {
            // 非单一确定类型
        };

        /**
         如果为true，则此属性指定消息的发送方式应确保传输协议确保远程端点接收到所有数据。更改消息上的msgReliable属性仅适用于启用选择属性perMsgReliability建立的连接。如果不是这种情况，更改msgReliable将产生错误。
禁用此属性表示传输服务系统可以禁用此特定消息的重传或其他可靠性机制，但不能保证此类禁用。
如果应用程序在发送之前未配置此属性，则此属性的默认值将基于与发送操作关联的连接的选择属性可靠性。
         */

        struct ReliableDataTransfer
        {
            /*
            Name: msgReliable
            Type: Boolean
            Default: the queried Boolean value of the Selection Property reliability
            (Section 6.2.1)
            */
            // NOTE: 默认值需要解析 其他属性...
        };

        /**
此枚举属性指定应用程序发送此消息的首选权衡；它是ConnCapacityProfile连接属性的每条消息覆盖（请参阅第8.1.6节）。
如果应用程序在发送之前未对其进行配置，则此属性的默认值将基于与发送操作关联的连接的连接属性ConnCapacityProfile。
         */
        struct MessageCapacityProfileOverride
        {
        };

        /**
         * @brief
         * 当需要在单个网络数据包中发送消息时，将使用非分段。
如果可能，此属性指定应在没有网络层碎片的情况下发送和接收消息。当首选传输分段时，它可用于避免网络层碎片。

这仅在传输使用支持此功能的网络层时生效。当它生效时，将此属性设置为true将导致发送方避免网络层源碎片。当使用IPv4时，这将导致在IP标头中设置不要碎片（DF）位。

尝试使用此属性发送导致大小大于传输当前估计的最大数据包大小（singularTransmissionMsgMaxLen）的消息可能会在允许时或在SendError中导致传输分段。
         */
        struct NoNetworkLayerFragmentation
        {
            /**
                Name: noFragmentation
                Type: Boolean
                Default: false
             */
            static constexpr std::string_view name = "noFragmentation"; // NOLINT
            using type = __detail::Boolean;
            type value = false;
        };

        struct NoSegmentation
        {
            static constexpr std::string_view name = "noSegmentation"; // NOLINT
            using type = __detail::Boolean;
            type value = false;
        };
    };

}; // namespace mcs::taps::data