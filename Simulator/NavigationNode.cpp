#include "NavigationNode.h"
#include <iostream>
    //getters
const std::string& NavigationNode::getNavigationName() const{
    return navigation_name;
}
//const std::unordered_set<GISNode,GISNodehash>& NavigationNode::getGISNodes() const{
//    return gis_nodes;
//}
const std::unordered_map<std::string, GISNode>& NavigationNode::getGISNodes() const{
    return gis_nodes;
}

int NavigationNode::getScore() const{
    return score;
}
bool NavigationNode::getIsValidDistance() const{
    return is_valid_distance;
}
bool NavigationNode::getIsValidTime() const{
    return is_valid_time;
}
size_t NavigationNode::getBestGISCounterDistance() const {
    //return best_gis_counter_distance;
    return distance_gis_node.getGISCounter();
}
size_t NavigationNode::getBestGISCounterTime() const {
    //return best_gis_counter_time;
    return time_gis_node.getGISCounter();
}
const AbstractRoute& NavigationNode::getDistanceRoute() const {
    return distance_gis_node.getDistanceRoute();
}
const AbstractRoute& NavigationNode::getTimeRoute() const {
    return time_gis_node.getTimeRoute();
}
 const GISNode& NavigationNode::getDistanceGISNode() const{
    return distance_gis_node;
}
 const GISNode& NavigationNode::getTimeGISNode()const{
    return time_gis_node;
}
//setters
void NavigationNode::addToScore(const int point){
    score += point;
}
void NavigationNode::addGISNode(GISNode&& gis_node){
    if (!gis_node.getIsValidDistance()) {
        is_valid_distance = false;
    }
    if (!gis_node.getIsValidTime()) {
        is_valid_time = false;
    }
    gis_nodes.insert({gis_node.getGISName(),std::move(gis_node)});
}
void NavigationNode::setIsValidDistance(const bool is_valid) {
    is_valid_distance = is_valid;
}
void NavigationNode::setIsValidTime(const bool is_valid) {
    is_valid_time = is_valid;
}   

void NavigationNode::updateValidGISConsensusTime(const std::string& gis_name) {
    gis_nodes.find(gis_name)->second.setInConsensusTime(true);
}
void NavigationNode::updateValidGISConsensusDistance(const std::string& gis_name) {
    gis_nodes.find(gis_name)->second.setInConsensusDistance(true);
}
void NavigationNode::updateRouteDistance(const std::string& gis_name) {
    auto& gis_node = gis_nodes.find(gis_name)->second;
   NavigationNode::updateRouteANDCounterDistance(  distance_gis_node,
                                    gis_node);
}

void NavigationNode::updateRouteTime(const std::string& gis_name) {
    auto& gis_node = gis_nodes.find(gis_name)->second;
    NavigationNode::updateRouteANDCounterTime(  time_gis_node,
                                gis_node);       
}
