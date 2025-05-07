#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::tfil
{
    /**
     * irregular    = "en-GB-oed"         ; irregular tags do not match
                    / "i-ami"             ; the 'langtag' production and
                    / "i-bnn"             ; would not otherwise be
                    / "i-default"         ; considered 'well-formed'
                    / "i-enochian"        ; These tags are all valid,
                    / "i-hak"             ; but most are deprecated
                    / "i-klingon"         ; in favor of more modern
                    / "i-lux"             ; subtags or subtag
                    / "i-mingo"           ; combination
                    / "i-navajo"
                    / "i-pwn"
                    / "i-tao"
                    / "i-tay"
                    / "i-tsu"
                    / "sgn-BE-FR"
                    / "sgn-BE-NL"
                    / "sgn-CH-DE"
     *
     */
    using irregular =
        alternative<String<"en-GB-oed">, String<"i-ami">, String<"i-bnn">,
                    String<"i-default">, String<"i-enochian">, String<"i-hak">,
                    String<"i-klingon">, String<"i-lux">, String<"i-mingo">,
                    String<"i-navajo">, String<"i-pwn">, String<"i-tao">, String<"i-tay">,
                    String<"i-tsu">, String<"sgn-BE-FR">, String<"sgn-BE-NL">,
                    String<"sgn-CH-DE">>;

}; // namespace mcs::abnf::tfil