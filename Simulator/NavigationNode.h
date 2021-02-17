#pragma once
#include "../Common/GISNamedTypes.h"
#include "GISNode.h"
#include "../UserCommon/Hashers.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <memory>

class NavigationNode{

    std::string                                 navigation_name;
    int                                         score;
    bool                                        is_valid_distance;
    bool                                        is_valid_time;
    std::unordered_map<std::string, GISNode>    gis_nodes;   
    GISNode                                     distance_gis_node;
    GISNode                                     time_gis_node;


    public:

    NavigationNode( const std::string&          navigation_name_
                    )
                    :
                    navigation_name             (navigation_name_),
                    score                       (0),
                    is_valid_distance           (true),
                    is_valid_time               (true)  {}

	

    //getters
    const std::string& getNavigationName() const;
    const std::unordered_map<std::string, GISNode>& getGISNodes() const;

    int getScore() const;
    bool getIsValidDistance() const;
    bool getIsValidTime() const;

    size_t getBestGISCounterDistance() const;
    size_t getBestGISCounterTime() const ;
    const AbstractRoute& getDistanceRoute() const;
    const AbstractRoute& getTimeRoute() const;
     const GISNode& getDistanceGISNode()const ;
     const GISNode& getTimeGISNode()const;

    //setters
    void addToScore(const int point);
    void addGISNode(GISNode&& gis_node);
       
    void setIsValidDistance(const bool is_valid);
    void setIsValidTime(const bool is_valid);

    void updateValidGISConsensusTime(const std::string& gis_name);

    void updateValidGISConsensusDistance(const std::string& gis_name);

    void updateRouteDistance(const std::string& gis_name);
    
    void updateRouteTime(const std::string& gis_name);

    //left is smaller: return -1
    //right is smaller: return 1
    //equals: return 0
static int compareRoutesByDistance(const AbstractRoute& left, const AbstractRoute& right) {
    if (left.totalLength() < right.totalLength()) {
        return -1;
    }
    else if (left.totalLength() == right.totalLength()) {
        if (left.estimatedDuration() < right.estimatedDuration()) {
            return -1;
        }
        else if (left.estimatedDuration() == right.estimatedDuration()) {
            return 0;
        }
    }
    return 1;
}
    //left is smaller: return -1
    //right is smaller: return 1
    //equals: return 0
static int compareRoutesByTime(const AbstractRoute& left, const AbstractRoute& right) {
    if (left.estimatedDuration() < right.estimatedDuration()) {
        return -1;
    }
    else if (left.estimatedDuration() == right.estimatedDuration()) {
        if (left.totalLength() < right.totalLength()) {
            return -1;
        }
        else if (left.totalLength() == right.totalLength()) {
            return 0;
        }
    }
    return 1;
}    
    
static void updateRouteANDCounterDistance(  GISNode&    old_gis,
                                             const GISNode&    new_gis)
{
    if (old_gis.getGISName().compare("") != 0) {
        int new_is_smaller = compareRoutesByDistance(new_gis.getDistanceRoute(), old_gis.getDistanceRoute());
        if (new_is_smaller == -1) {
            old_gis = new_gis;
        }
        else if (new_is_smaller == 0) {
            if (new_gis.getGISCounter() < old_gis.getGISCounter()) {
                old_gis = new_gis;
                
            }
        }
    }
    else {
        old_gis = new_gis;
    }
}
static void updateRouteANDCounterTime(  GISNode&    old_gis,
                                             const GISNode&    new_gis)
{
    if (old_gis.getGISName().compare("") != 0) {
        int new_is_smaller = compareRoutesByTime(new_gis.getTimeRoute(), old_gis.getTimeRoute());
        if (new_is_smaller == -1) {
            old_gis = new_gis;
        }
        else if (new_is_smaller == 0) {
            if (new_gis.getGISCounter() < old_gis.getGISCounter()) {
                old_gis = new_gis;               
            }
        }
    }
    else {
        old_gis = new_gis;        
    }
}

};