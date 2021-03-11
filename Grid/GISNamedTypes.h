#pragma once 

#include "NamedType.h"
#include "Double.h"
#include <string>

constexpr std::size_t coordinate_precision = 6;
template<typename Type>
using Coordinate = NamedTypeDouble<coordinate_precision, Type>;
struct Longitude: Coordinate<Longitude> { using Coordinate<Longitude>::Coordinate; };
struct Latitude: Coordinate<Latitude> { using Coordinate<Latitude>::Coordinate; };

template<typename T>
concept LongLat = std::same_as<T, Longitude> || std::same_as<T, Latitude>;

constexpr std::size_t meters_precision = 2;
struct Meters: NamedTypeDouble<meters_precision, Meters> {
    using NamedTypeDouble<meters_precision, Meters>::NamedTypeDouble;
};

constexpr std::size_t minutes_precision = 2;
struct Minutes: NamedTypeDouble<minutes_precision, Minutes> {
    using NamedTypeDouble<minutes_precision, Minutes>::NamedTypeDouble;
};

class Coordinates {
    Longitude _longitude;
    Latitude _latitude;
public:
    Coordinates(Longitude longitude, Latitude latitude) : _longitude(longitude), _latitude(latitude) {}
    Longitude longitude() const { return _longitude; }
    Latitude latitude () const { return _latitude; }
    bool operator==(const Coordinates& rhs) const {
           return _longitude == rhs._longitude && _latitude == rhs._latitude;
    }
    template<std::size_t index>
    friend auto get(const Coordinates& c) {
        if constexpr(index == 0) {
            return c.longitude();
        }
        else {
            return c.latitude();
        }
    }    
};

namespace std {
    template<>
    class tuple_size<Coordinates>
        : public std::integral_constant<std::size_t, 2> { };

    template<>
    class tuple_element<0, Coordinates> {
    public:
        using type = Longitude;
    };

    template<>
    class tuple_element<1, Coordinates> {
    public:
        using type = Latitude;
    };

    template<>
    struct hash<Coordinates> {
	 std::size_t operator()(const Coordinates& c) const {
		std::size_t h1 = std::hash<double>{}(c.longitude()->roundToPrecision());
		std::size_t h2 = std::hash<double>{}(c.latitude()->roundToPrecision());
		return h1 ^ (h2 << 1); 
        }
    };
}

struct EntityId: NamedType<std::string> {
    using NamedType<std::string>::NamedType;
};

namespace std {
    template<>
    struct hash<EntityId> {
	std::size_t operator()(const EntityId& t) const {
            	return std::hash<std::string>{}(static_cast<const std::string&>(t));
        }
    };
}

enum class Direction {A_to_B, B_to_A}; 