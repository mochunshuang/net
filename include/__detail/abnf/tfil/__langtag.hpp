#pragma once

#include "./__language.hpp"
#include "./__script.hpp"
#include "./__region.hpp"
#include "./__variant.hpp"
#include "./__extension.hpp"
#include "./__privateuse.hpp"

namespace mcs::abnf::tfil
{
    /**
     *  langtag       = language
                 ["-" script]
                 ["-" region]
                 *("-" variant)
                 *("-" extension)
                 ["-" privateuse]
     *
     */
    using langtag = sequence<language, optional<sequence<Char<'-'>, script>>,
                             optional<sequence<Char<'-'>, region>>,
                             zero_or_more<sequence<Char<'-'>, variant>>,
                             zero_or_more<sequence<Char<'-'>, extension>>,
                             optional<sequence<Char<'-'>, privateuse>>>;

}; // namespace mcs::abnf::tfil