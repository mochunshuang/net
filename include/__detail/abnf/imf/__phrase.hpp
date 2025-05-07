#pragma once

#include "./__obs_phrase.hpp"
#include "./__word.hpp"

namespace mcs::abnf::imf
{
    // NOTE: 调整顺序，特别对alternative 最特殊最该优先处理
    //  obs-phrase      =   word *(word / "." / CFWS)
    //  phrase          =   1*word / obs-phrase
    using phrase = alternative<obs_phrase, one_or_more<word>>;
}; // namespace mcs::abnf::imf