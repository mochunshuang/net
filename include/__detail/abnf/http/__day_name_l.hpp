#pragma once

#include "../__core_types.hpp"
#include <cstdint>

namespace mcs::abnf::http
{
    /*
    day-name-l = %x4D.6F.6E.64.61.79 ; Monday
                / %x54.75.65.73.64.61.79 ; Tuesday
                / %x57.65.64.6E.65.73.64.61.79 ; Wednesday
                / %x54.68.75.72.73.64.61.79 ; Thursday
                / %x46.72.69.64.61.79 ; Friday
                / %x53.61.74.75.72.64.61.79 ; Saturday
                / %x53.75.6E.64.61.79 ; Sunday
    */
    constexpr abnf_result auto day_name_l(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();

        if (constexpr auto k_length = 6; k_size == k_length)
        {
            static constexpr OCTET k_Monday[k_length] = {'M', 'o', 'n', // NOLINT
                                                         'd', 'a', 'y'};
            static constexpr OCTET k_Friday[k_length] = {'F', 'r', 'i', // NOLINT
                                                         'd', 'a', 'y'};
            static constexpr OCTET k_Sunday[k_length] = {'S', 'u', 'n', // NOLINT
                                                         'd', 'a', 'y'};
            uint8_t i{0};
            while (i < k_length && sp[i] == k_Monday[i]) // NOLINT
                ++i;
            if (i == k_length)
                return true;

            i = 0;
            while (i < k_length && sp[i] == k_Friday[i]) // NOLINT
                ++i;
            if (i == k_length)
                return true;

            i = 0;
            while (i < k_length && sp[i] == k_Sunday[i]) // NOLINT
                ++i;
            if (i == k_length)
                return true;
        }
        if (constexpr auto k_length = 7; k_size == k_length)
        {
            static constexpr OCTET k_Tuesday[k_length] = {'T', 'u', 'e', // NOLINT
                                                          's', 'd', 'a', 'y'};
            uint8_t i{0};
            while (i < k_length && sp[i] == k_Tuesday[i]) // NOLINT
                ++i;
            if (i == k_length)
                return true;
        }
        if (constexpr auto k_length = 8; k_size == k_length)
        {
            static constexpr OCTET k_Thursday[k_length] = {'T', 'h', 'u', 'r', // NOLINT
                                                           's', 'd', 'a', 'y'};
            static constexpr OCTET k_Saturday[k_length] = {'S', 'a', 't', 'u', // NOLINT
                                                           'r', 'd', 'a', 'y'};
            uint8_t i{0};
            while (i < k_length && sp[i] == k_Thursday[i]) // NOLINT
                ++i;
            if (i == k_length)
                return true;
            i = 0;
            while (i < k_length && sp[i] == k_Saturday[i]) // NOLINT
                ++i;
            if (i == k_length)
                return true;
        }
        if (constexpr auto k_length = 9; k_size == k_length)
        {
            static constexpr OCTET k_Wednesday[k_length] = {'W', 'e', 'd', 'n', // NOLINT
                                                            'e', 's', 'd', 'a', 'y'};
            uint8_t i{0};
            while (i < k_length && sp[i] == k_Wednesday[i]) // NOLINT
                ++i;
            if (i == k_length)
                return true;
        }
        return false;
    }
}; // namespace mcs::abnf::http