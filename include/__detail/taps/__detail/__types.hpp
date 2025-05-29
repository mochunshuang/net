#pragma once

#include <string>

namespace mcs::taps::__detail
{
    /**
     * @see https://www.rfc-editor.org/rfc/rfc9622.html#name-terminology-and-notation
     *
     */
    using Boolean = bool;

    using Integer = int;

    // Numeric: Instances take real number values.
    //  using Numeric =

    // String: Instances are represented in UTF-8.
    using String = std::u8string;

    /*
        Enumeration:
        A family of types in which each instance takes one of a fixed, predefined set of
        values specific to a given enumerated type
    */

    // using Enumeration =

    // using Tuple = std::tuple;

    // Array
    // Set

}; // namespace mcs::taps::__detail