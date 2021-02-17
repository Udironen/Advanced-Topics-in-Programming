#pragma once
#include "Route.h"
#include "../Common/AbstractRoutes.h"

class Routes : public AbstractRoutes {

    static const std::string erorr_msg;

    Route shortest_distance;
    Route shortest_time;
    bool is_valid;



public:
    //constructor
    Routes(const Route& shortest_distance_, const  Route& shortest_time_, const bool is_valid_) :
        shortest_distance(shortest_distance_), shortest_time(shortest_time_), is_valid(is_valid_) {}

    Routes(Route&& shortest_distance_, Route&& shortest_time_, const bool is_valid_) :
        shortest_distance(std::move(shortest_distance_)), shortest_time(std::move(shortest_time_)), is_valid(is_valid_) {}


    Routes(const Routes& routes) = default;

    Routes(Routes&& routes) = default;

    ~Routes() = default;

    Routes& operator=(const Routes& routes) = default;

    Routes& operator=(Routes&& routes) = default;

       


    bool operator!= (const Routes& routes) {
        return !(*this == routes);
    }

    bool operator== (const Routes& routes) {
        return ((is_valid == routes.is_valid) &&
            (shortest_distance == routes.shortest_distance) &&
            (shortest_time == routes.shortest_time));
    }

    bool isValid() const {
        return is_valid;
    }

    const AbstractRoute& shortestDistance() const {
        return shortest_distance;
    }

    const AbstractRoute& shortestTime() const {
        return shortest_time;
    }

    const std::string& getErrorMessage() const {
        return erorr_msg;
    }


};
