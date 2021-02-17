#pragma once
#include <vector>
#include "../Common/GISNamedTypes.h"
#include "NavigationNode.h"
#include <list>
#include <iostream>
class RequestNode {
	Coordinates						start;
	Coordinates						end;
	std::vector<NavigationNode>		navi_nodes;
	GISNode 						best_distance_gis_node;
	GISNode 						best_time_gis_node;
	bool							is_valid;
	

public:
	RequestNode(const Coordinates&	start_,
				const Coordinates&	end_) 
				:
				start						(start_),
				end							(end_),			
				is_valid					(true){}
	

	~RequestNode() = default;

	Coordinates getStart() const;

	Coordinates getEnd() const;

	 const std::vector<NavigationNode>& getNavigationNodes() const ;
	  std::vector<NavigationNode>& getNavigationNodes()  ;
	const AbstractRoute& getBestRouteDistance() const;

	const AbstractRoute& getBestRouteTime() const;

	std::size_t getBestGISCounterDistance() const;

	std::size_t getBestGISCounterTime() const;

	void addNavigationNode(NavigationNode&& navi_node);

	bool getIsValid()const;

	void setValid(const bool is_valid_);

	void insertGISNode(const size_t index, GISNode&& gis_node);

	void updateBestRouteDistance(const NavigationNode& navi_node);

	void updateBestRouteTime(const NavigationNode& navi_node);
};