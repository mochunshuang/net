#pragma once

#include "./__transfer_coding.hpp"
#include "./__weight.hpp"
#include "__tchar.hpp"

namespace mcs::abnf::http
{
    // weight = OWS ";" OWS "q=" qvalue
    // t-codings = "trailers" / ( transfer-coding [ weight ] )
    constexpr abnf_result auto t_codings(span_param_in sp) noexcept
    {
        using builder = result_builder<result<2>>;
        const auto k_size = sp.size();
        if (k_size == 0)
            return builder::fail(0);

        if (constexpr uint8_t k_trailers_size = 8; k_size == k_trailers_size)
        {
            constexpr OCTET k_trailers[k_trailers_size] = {'t', 'r', 'a', 'i', // NOLINT
                                                           'l', 'e', 'r', 's'};
            uint8_t i{0};
            while (i < k_trailers_size && sp[i] == k_trailers[i]) // NOLINT
                ++i;
            if (i == k_trailers_size)
                return builder::success(span{.start = 0, .count = k_size}, invalid_span);
        }

        static_assert(not tchar('='));
        static_assert(VCHAR('='));

        if (transfer_coding(sp))
            return builder::success(span{.start = 0, .count = k_size}, invalid_span);

        // qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
        // weight = OWS ";" OWS "q=" qvalue
        std::size_t index = k_size;
        std::size_t index_0 = k_size; // index_;
        std::size_t index_1 = k_size; // index_q
        constexpr int k_fund_q_max_times = 7;
        for (int fund_q_times = 0; index-- > 0;) // NOLINT
        {
            if (sp[index] == ';' && index_0 == k_size) // find ';'
            {
                index_0 = index;
                break;
            }

            if (index_1 != k_size) // only need find ';'
            {
                // OWS = *( SP / HTAB )
                if (sp[index] == SP || sp[index] == HTAB) //  in [index_; ,index_q]
                    continue;
                return builder::fail(0);
            }

            if (sp[index] == '=' && index > 0 && sp[index - 1] == 'q' &&
                index_1 == k_size) // find "q="
            {
                if (index_0 != k_size) // need not find ';' before
                    return builder::fail(0);
                index_1 = index - 1;
            }

            if (fund_q_times > k_fund_q_max_times && index_1 == k_size)
                return builder::fail(0);
            fund_q_times++;
        }
        // not find or only find one or no transfer_coding span
        if (index_0 == k_size || index_1 == k_size || index_0 == 0)
            return builder::fail(0);

        std::size_t end_transfer = index_0 - 1; // remove OWS in (end_transfer,index_0)
        while (end_transfer > 0 && (sp[end_transfer] == SP || sp[end_transfer] == HTAB))
            --end_transfer;
        static_assert(not tchar(SP) && not tchar(HTAB));

        const auto k_transfer = sp.first(end_transfer + 1);
        const auto k_weight = sp.subspan(index_0);
        if (transfer_coding(k_transfer) && weight(k_weight))
            return builder::success(span{.start = 0, .count = end_transfer + 1},
                                    span{.start = index_0, .count = k_size - index_0});

        return builder::fail(0);
    }
}; // namespace mcs::abnf::http