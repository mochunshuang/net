#pragma once

#include <typeinfo>
namespace mcs::net::ip
{
    class bad_address_cast : public std::bad_cast // NOLINT
    {
        [[nodiscard]] const char *what() const noexcept override
        {
            return "bad address cast";
        }
    };
}; // namespace mcs::net::ip