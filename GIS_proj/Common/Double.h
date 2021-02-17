#pragma once 

#include "NamedType.h"
#include <cmath>
#include <tuple>
#include <concepts>

template<std::size_t I>
consteval double tenToThePowerOf() {
    if constexpr(I == 0) return 1;
    else return 10 * tenToThePowerOf<I - 1>();
}

template<std::size_t I>
consteval double tenToThePowerOfMinus() {
    if constexpr(I == 0) return 1;
    else return 0.1 * tenToThePowerOfMinus<I - 1>();
}

template<std::size_t Precision>
class Double {
    double _val;

    template<std::size_t Precision2>
    friend auto comparisonArgs(Double<Precision> lhs, Double<Precision2> rhs) {
        static constexpr std::size_t actual_precision_factor = std::max(lhs.precision_factor, rhs.precision_factor);
        double val1 = lhs.roundToPrecision();
        double val2 = rhs.roundToPrecision();
        return std::pair{static_cast<long>(val1 * actual_precision_factor), static_cast<long>(val2 * actual_precision_factor)};
    }

public:
    static constexpr size_t precision = Precision;
    static constexpr double precision_factor = tenToThePowerOf<Precision>();
    
    constexpr Double(double val) : _val(val) {}
    
    constexpr double roundToPrecision() const {
        long whole = static_cast<long>(_val);
        double fraction = _val - whole;
        double rounded_fraction = std::round(fraction * precision_factor) / precision_factor;
        return whole + rounded_fraction;
    }

    template<std::size_t Precision2>
    friend constexpr auto operator<=>(Double<Precision> lhs, Double<Precision2> rhs) {
        auto [val1, val2] = comparisonArgs(lhs, rhs);
        return val1 <=> val2;
    }
    
    template<std::size_t Precision2>
    friend constexpr bool operator==(Double<Precision> lhs, Double<Precision2> rhs) {
        auto [val1, val2] = comparisonArgs(lhs, rhs);
        return val1 == val2;
    }
    
    constexpr operator double() const {
        return _val;
    }
};

template<typename T>
concept Number = std::integral<T> || std::floating_point<T> || std::same_as<T, Double<T::precision>>;

template<std::size_t Precision, typename Type = Double<Precision>>
struct NamedTypeDouble : NamedType<Double<Precision>> {
    using NamedType<Double<Precision>>::NamedType;

    //---------------------------------------------
    // NOTE: the cast to double is now explicit
    // if you need a cast to double you should do
    // an implicit casting!
    //---------------------------------------------
    constexpr explicit operator double() const {
        return static_cast<const Double<Precision>&>(*this);
    };

    friend constexpr Type& operator+=(Type& d1, Type d2) {
        d1 = d1 + d2;
        return d1;
    }

    friend constexpr Type& operator-=(Type& d1, Type d2) {
        d1 = d1 - d2;
        return d1;
    }

    friend constexpr Type operator+(Type d1, Type d2) {
        return Type { d1.val + d2.val };
    }

    friend constexpr Type operator-(Type d1, Type d2) {
        return Type { d1.val - d2.val };
    }

	friend constexpr bool operator==(Type d1, Type d2) {
	    return d1.val == d2.val;
	}
    
	friend constexpr auto operator<=>(Type d1, Type d2) {
	    return d1.val <=> d2.val;
	}

    friend constexpr Type operator*(Type d1, Number auto d2) {
        return Type { d1.val * d2 };
    }

    friend constexpr Type operator*(Number auto d1, Type d2) {
        return Type { d1 * d2.val };
    }

    friend constexpr Type operator/(Type d1, Number auto d2) {
        return Type { d1.val / d2 };
    }

    friend constexpr double operator/(Type d1, Type d2) {
        return d1.val / d2.val;
    }

    // Note that the base class doesn't support the operation: Type * Type
    // the actual type may allow it (e.g. Meters * Meters => SquareMeters)
    
    friend constexpr bool operator==(Type lhs, auto rhs) = delete;
    friend constexpr bool operator==(auto lhs, Type rhs) = delete;
    friend constexpr bool operator<=>(Type lhs, auto rhs) = delete;
    friend constexpr bool operator<=>(auto lhs, Type rhs) = delete;
};