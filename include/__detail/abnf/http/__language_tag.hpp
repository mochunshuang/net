#pragma once

#include "../tfil/__language_tag.hpp"

namespace mcs::abnf::http
{
    // language-tag = <Language-Tag, see [RFC5646], Section 2.1>
    using language_tag = tfil::Language_Tag;
}; // namespace mcs::abnf::http
