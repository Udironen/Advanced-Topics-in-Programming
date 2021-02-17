#pragma once
#include "../Common/GISNamedTypes.h"
#include "../Common/AbstractWay.h"
#include "../Common/CoordinatesMath.h"
#include "../Common/AbstractRoute.h"
#include "Node.h"
#include <vector>

class Route : public AbstractRoute  {

    friend class NavigationGIS;
    std::vector<std::pair<EntityId, Direction>> all_ways;
    Coordinates start_point;
    Coordinates end_point;
    Meters total_length;
    Minutes est_duration;

    
public:

    // constructor
    Route() :
        start_point(Coordinates(Longitude(Node::defult_longi), Latitude(Node::defult_lati))),
        end_point(Coordinates(Longitude(Node::defult_longi), Latitude(Node::defult_lati))),
        total_length(Meters(0)),
        est_duration(Minutes(0)) {}

    Route(const std::vector<std::pair<EntityId, Direction>>& all_ways_,
        const Coordinates& start_, const Coordinates& end_,
        const Meters& total_length_, const Minutes& est_duration_) :
        all_ways(all_ways_), start_point(start_),
        end_point(end_), total_length(total_length_),
        est_duration(est_duration_) {}

    Route(std::vector<std::pair<EntityId, Direction>>&& all_ways_,
        const Coordinates& start_, const Coordinates& end_,
        const Meters& total_length_, const Minutes& est_duration_) noexcept :
        start_point(start_),
        end_point(end_), total_length(total_length_),
        est_duration(est_duration_) {

        std::swap(all_ways, all_ways_);
    }
   
    Route(const Route& route) = default;

    Route(Route&& route) noexcept :
        start_point(route.start_point),
        end_point(route.end_point), total_length(route.total_length),
        est_duration(route.est_duration) {
        std::swap(all_ways, route.all_ways);
    }
    
    ~Route() = default;

    Route& operator= (const Route& route) = default;

    Route& operator= (Route&& route) noexcept {
        std::swap(all_ways, route.all_ways);
        start_point = route.start_point;
        end_point = route.end_point;
        total_length = route.total_length;
        est_duration = route.est_duration;

        return *this;
    }

    bool operator!= (const Route& route) const {
        return !(std::equal(all_ways.begin(), all_ways.end(), route.all_ways.begin()));
    }

    bool operator== (const Route& route) const {
        return std::equal(all_ways.begin(), all_ways.end(), route.all_ways.begin());
    }


    const std::vector<std::pair<EntityId, Direction>>& getWays() const {
        return all_ways;
    }

    const Coordinates& getWayStartPoint() const {
        return start_point;
    }

    const Coordinates& getWayEndPoint() const {
        return end_point;
    }

    Meters totalLength() const {
        return total_length;
    }

    Minutes estimatedDuration() const {
        return est_duration;
    }
    
};