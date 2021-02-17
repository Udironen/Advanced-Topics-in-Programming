#include "GISNode.h"


const std::string& GISNode::getGISName() const{
    return gis_name;
}
const AbstractRoute& GISNode::getDistanceRoute() const{
    return routes->shortestDistance();
}
const AbstractRoute& GISNode::getTimeRoute() const {
    return routes->shortestTime();
}
bool GISNode::getIsValidDistance() const {
    return is_valid_distance;
}
bool GISNode::getIsValidTime() const {
    return is_valid_time;
}
bool GISNode::getInConsensusTime() const {
    return in_consensus_time;
}
bool GISNode::getInConsensusDistance() const {
    return in_consensus_distance;
}
size_t GISNode::getGISCounter() const {
    return gis_counter;
}

void GISNode::setIsValidDistance(const bool is_valid) {
    is_valid_distance = is_valid;
}
void GISNode::setInvalidTime(const bool is_valid) {
    is_valid_time = is_valid;
}
void GISNode::setInConsensusTime(const bool in_consensus_) {
    in_consensus_time = in_consensus_;
}
void GISNode::setInConsensusDistance(const bool in_consensus_) {
    in_consensus_distance = in_consensus_;
}
