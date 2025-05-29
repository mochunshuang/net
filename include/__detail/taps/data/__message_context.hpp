#pragma once

namespace mcs::taps::data
{
    /*
MessageContext.add(property, value)
PropertyValue := MessageContext.get(property)
RemoteEndpoint := MessageContext.GetRemoteEndpoint()
LocalEndpoint := MessageContext.GetLocalEndpoint()

RemoteEndpoint := MessageContext.GetRemoteEndpoint()
LocalEndpoint := MessageContext.GetLocalEndpoint()
*/
    struct message_context
    {
        // using property_value_type = int;
        // using property_key_type = int;
        // NOTE: add_f
        /**
         * @brief
            messageContext := NewMessageContext()
            messageContext.add(framer, key, value)
            Connection.Send(messageData, messageContext)
         */
    };
}; // namespace mcs::taps::data