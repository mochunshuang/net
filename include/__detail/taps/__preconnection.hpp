#pragma once

#include "./__transport_properties.hpp"
#include "./__security_parameters.hpp"

namespace mcs::taps
{
    struct preconnection
    {
        //
        //
        /**
         * Preestablishment Phase
        * @see https://www.rfc-editor.org/rfc/rfc9622.html#name-preestablishment-phase
           Preconnection := NewPreconnection([]LocalEndpoint,
                                     []RemoteEndpoint,
                                     TransportProperties,
                                     SecurityParameters)
         */
        // preconnection(local_endpoint l_e, remote_endpoint r_e, transport_properties
        // t_p,
        //               security_parameters s_p);
    };
}; // namespace mcs::taps