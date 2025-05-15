#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::tfil
{
    /**
     *  regular       = "art-lojban"        ; these tags match the 'langtag'
                        / "cel-gaulish"       ; production, but their subtags
                        / "no-bok"            ; are not extended language
                        / "no-nyn"            ; or variant subtags: their meaning
                        / "zh-guoyu"          ; is defined by their registration
                        / "zh-hakka"          ; and all of these are deprecated
                        / "zh-min"            ; in favor of a more modern
                        / "zh-min-nan"        ; subtag or sequence of subtags
                        / "zh-xiang"
     *
     */
    using regular = // "zh-min-nan" before "zh-min" is need
        alternative<String<"art-lojban">, String<"cel-gaulish">, String<"no-bok">,
                    String<"no-nyn">, String<"zh-guoyu">, String<"zh-hakka">,
                    String<"zh-min-nan">, String<"zh-min">, String<"zh-xiang">>;

}; // namespace mcs::abnf::tfil