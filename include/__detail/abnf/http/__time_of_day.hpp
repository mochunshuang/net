#pragma once

#include "./__hour.hpp"
#include "./__minute.hpp"
#include "./__second.hpp"

namespace mcs::abnf::http
{
    // hour = 2DIGIT
    // minute = 2DIGIT
    // second = 2DIGIT
    // time-of-day = hour ":" minute ":" second
    constexpr abnf_result auto time_of_day(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size != 8) // NOLINT
            return simple_result::fail(0);

        std::size_t i{0};
        if (not hour(sp[i], sp[i + 1])) // hour
            return simple_result::fail(i);
        i += 2;

        if (sp[i] != ':')
            return simple_result::fail(i);
        ++i;

        if (not minute(sp[i], sp[i + 1])) // minute
            return simple_result::fail(i);
        i += 2;

        if (sp[i] != ':')
            return simple_result::fail(i);
        ++i;

        if (not second(sp[i], sp[i + 1])) // minute
            return simple_result::fail(i);
        return simple_result::success(); // NOLINT
    }
}; // namespace mcs::abnf::http
