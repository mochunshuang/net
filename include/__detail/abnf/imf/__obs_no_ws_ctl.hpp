#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::imf
{
    /**
    * @brief  obs-NO-WS-CTL   =   %d1-8 /            ; US-ASCII control
                        %d11 /             ;  characters that do not
                        %d12 /             ;  include the carriage
                        %d14-31 /          ;  return, line feed, and
                        %d127              ;  white space characters
    *
    */
    using obs_NO_WS_CTL = // NOLINTNEXTLINE
        alternative<Range<1, 8>, Range<11, 12>, Range<14, 31>, Char<127>>;

}; // namespace mcs::abnf::imf