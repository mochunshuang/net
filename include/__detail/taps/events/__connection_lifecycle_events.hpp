#pragma once

namespace mcs::taps::events
{
    // https://www.rfc-editor.org/rfc/rfc9622.html#name-connection-lifecycle-events
    struct connection_lifecycle_events
    {
        struct SoftError
        {
        };

        struct PathChange
        {
        };
    };
}; // namespace mcs::taps::events