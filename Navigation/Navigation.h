#pragma once

#include "Routes.h"
#include "../Common/NavigationRegistration.h"
#include "../Common/NavigationGIS.h"
#include "../UserCommon/Hashers.h"
#include "../Common/AbstractNavigation.h"
#include "../Common/AbstractRoutes.h"
#include <queue>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <functional>



constexpr auto GOAL = "!GOAL!";


template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>,
    PairCompare > elements;

    inline bool empty() const {
        return elements.empty();
    }

    inline void put(T item, priority_t priority) {
        elements.emplace(priority, item);
    }

    T get() {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};


class Navigation_313452542 : public AbstractNavigation {
    
    //NavigationGIS gis;  
  
    //the algorithm to compute the best route:
    //by distance if is_time = false, 
    //by time if is_time = true.
    //goal: the destination node.
    //seen_node: map of <node_id, Node> of nodes that already have been found.
    //frontier: the priority queue.
    //last_way_id: the id of the way that goal is on it.      
    bool aStarSearch(Node& goal,
        std::unordered_map<EntityId, Node, IDhash>& seen_nodes,
        std::unordered_map<EntityId,const AbstractWay&, IDhash>& seen_ways,
        PriorityQueue<iter_node, double>& frontier,
        const std::pair<Meters,Meters>& dist_from_to, const bool is_time = false) const;

    //resets node priority  
    void resetNode(Node& node)const;

    //construct path by goal parents
    void constructPath(std::vector<std::pair<EntityId, Direction>>& all_ways, const Node& goal) const;

    //update the neighbors of the node by the aStar algorithm
    void updateNeighbors(const Node& node,
        std::unordered_map<EntityId, Node, IDhash>& seen_nodes,
        std::unordered_map<EntityId,const AbstractWay&, IDhash>& seen_ways,
        PriorityQueue<iter_node, double>& frontier,
        const std::pair<Meters, Meters>& dist_from_to,
        const Node& goal, const bool is_time = false) const;

    //return the correct direction according to from_to and junc_id
    Direction getDirection(
        const std::pair<EntityId, EntityId>& from_to, const EntityId& junc_id) const;

    //return the correct EntityId of the current neighbor according to from_to and junc_id
    const EntityId& getNeighborId(
        const std::pair<EntityId, EntityId>& from_to, const Direction direction) const;

    //return the correct Coordinates of the current neighbor according to from_to and junc_id
    Coordinates getNeighborCoords(
        const AbstractWay& way, const Direction direction) const;

    //return true  if start_coor_id and end_coor_id on the same way
    bool isSameWay(
        const std::tuple<Coordinates, EntityId, std::size_t>& start_tuple,
        const std::tuple<Coordinates, EntityId, std::size_t>& end_tuple) const;

    bool isValidPoint(const AbstractWay& way, const Coordinates& point, const Coordinates& coordinate) const;
        



public:

    

    //constractor
    Navigation_313452542(const NavigationGIS& gis_); 
    
    ~Navigation_313452542() = default;

  //  method gets a start point(that might be on a way or not) and an end point.The return value is a Routes object holding shortest by distance and shortest by time Route objects, each with the following data :
  //  -A vector of all ways to travel on, in proper order.
  //  - The start point on the first way closest to the source - this point must be a valid point on the first way in the vector(index 0) .
  //  - The end point on the last way closest to the destination - this point must be a valid point on the last way in the vector.
  //  - Distance and duration of the route(addition 15.12: from start point to end point as described above).
    std::unique_ptr<AbstractRoutes>  getRoutes(const Coordinates& start, const Coordinates& end) const;
   
};


