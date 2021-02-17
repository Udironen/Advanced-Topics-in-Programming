#pragma once
#include "../Common/AbstractRoutes.h"
//#include "NavigationNode.h"
#include <memory>
#include <iostream>

class GISNode{
    std::string gis_name;
    std::shared_ptr <AbstractRoutes>    routes;
    bool                                is_valid_distance;
    bool                                is_valid_time;
     bool                                in_consensus_time;
     bool                                in_consensus_distance;
    size_t                              gis_counter;

public:

    GISNode(const std::string&                  gis_name_,
            std::shared_ptr <AbstractRoutes>    routes_,
            bool                                is_valid_distance_,
            bool                                is_valid_time_,
            size_t                              gis_counter_)
            :
            gis_name            (gis_name_),
            routes              (std::move(routes_)),
            is_valid_distance   (is_valid_distance_),
            is_valid_time       (is_valid_time_),
            gis_counter         (gis_counter_)  {}
    
    GISNode(){}
    GISNode(const std::string& gis_name_) :gis_name(gis_name_){}  

    //getter
    
    const std::string& getGISName() const;

    const AbstractRoute& getDistanceRoute() const;

    const AbstractRoute& getTimeRoute() const;

    bool getIsValidDistance() const ;
    bool getIsValidTime() const;

    bool getInConsensusTime() const ;

    bool getInConsensusDistance() const;

    size_t getGISCounter() const;

    void setIsValidDistance(const bool is_valid);

    void setInvalidTime(const bool is_valid);

    void setInConsensusTime(const bool in_consensus_);
    void setInConsensusDistance(const bool in_consensus_);


};