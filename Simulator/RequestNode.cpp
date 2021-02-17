#include "RequestNode.h"

Coordinates RequestNode::getStart() const{
	return start;
}
Coordinates RequestNode::getEnd() const {
	return end;
}
 const std::vector<NavigationNode>& RequestNode::getNavigationNodes() const{
	return navi_nodes;
}
std::vector<NavigationNode>& RequestNode::getNavigationNodes() {
	return navi_nodes;
}
const AbstractRoute& RequestNode::getBestRouteDistance() const {
	return best_distance_gis_node.getDistanceRoute();
}
const AbstractRoute& RequestNode::getBestRouteTime() const {
	return best_time_gis_node.getTimeRoute();
}
std::size_t RequestNode::getBestGISCounterDistance() const {
	return best_distance_gis_node.getGISCounter();
}
std::size_t RequestNode::getBestGISCounterTime() const {
	return best_time_gis_node.getGISCounter();
}
void RequestNode::addNavigationNode(NavigationNode&& navi_node){
	navi_nodes.push_back(std::move(navi_node));
}	
bool RequestNode::getIsValid()const {
	return is_valid;
}
void RequestNode::setValid(const bool is_valid_) {
	is_valid = is_valid_;
}
void RequestNode::insertGISNode(const size_t index, GISNode&& gis_node) {
	navi_nodes[index].addGISNode(std::move(gis_node));
}
void RequestNode::updateBestRouteDistance(const NavigationNode& navi_node) {
	NavigationNode::updateRouteANDCounterDistance(	best_distance_gis_node,
													navi_node.getDistanceGISNode());
}
void RequestNode::updateBestRouteTime(const NavigationNode& navi_node) {
	NavigationNode::updateRouteANDCounterTime(	best_time_gis_node,
												navi_node.getTimeGISNode());
}