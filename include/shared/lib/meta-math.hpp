#pragma once

#include <stdint.h>

namespace meta_math {
    /// Calculate logarithms at compile-time.
    template <intmax_t Base, intmax_t Value>
    struct log
    {
        static constexpr intmax_t value = 1 + log<Base, Value/Base>::Exponent;
    };

    template <intmax_t Base>
    struct log<Base, 0>
    {
        static constexpr intmax_t value = 0;
    };

    template <intmax_t Base>
    struct log<Base, 1>
    {
        static constexpr intmax_t value = 0;
    };

    template <intmax_t Value>
    using log2 = log<2, Value>;

    template <intmax_t Value>
    using log10 = log<10, Value>;

    /// Calculate power at compile-time.
    template <intmax_t Base, intmax_t Exponent>
    struct pow
    {
        static constexpr intmax_t value = Base*pow<Base, Exponent - 1>::value;
    };

    template <intmax_t Base>
    struct pow<Base, 0>
    {
        static constexpr intmax_t value = 1;
    };

    template <intmax_t Base>
    struct pow<Base, 1>
    {
        static constexpr intmax_t value = Base;
    };
}
