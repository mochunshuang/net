#pragma once

namespace mcs::taps::concepts
{
    template <class E>
    concept is_endpoint = requires { typename E::endpoint_concept; };
}; // namespace mcs::taps::concepts