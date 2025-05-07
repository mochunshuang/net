#pragma once

#include "./__field_vchar.hpp"

namespace mcs::abnf::http
{
    /**
     * field-content = field-vchar [ 1*( SP / HTAB / field-vchar )
                                    field-vchar ]
     *
     */
    using field_content = sequence<
        field_vchar,
        optional<sequence<one_or_more<alternative<SP, HTAB, field_vchar>>, field_vchar>>>;
}; // namespace mcs::abnf::http