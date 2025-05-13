#pragma once

#include "./__origin_form.hpp"
#include "./__absolute_form.hpp"
#include "./__authority_form.hpp"
#include "./__asterisk_form.hpp"

namespace mcs::protocol::http::rules
{
    /**
     * request-target = origin-form / absolute-form / authority-form /
                        asterisk-form
     */
    using request_target =
        abnf::alternative<origin_form, absolute_form, authority_form, asterisk_form>;
}; // namespace mcs::protocol::http::rules