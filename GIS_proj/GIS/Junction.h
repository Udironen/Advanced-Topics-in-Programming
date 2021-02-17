#pragma once

#include "Entity.h"
 
#include <string>

class Junction: public Entity{
	
	Coordinates coor;

public:
	//constructors
	Junction(const EntityId& id_,
		const std::string& name_,
		const std::string& description_,
		const std::set<std::string>& tags_,
		const Coordinates& coor_);
	
	~Junction() = default;

	// get the longitude of junction
	Longitude getLongitude() const;

	// get the latitude of junction
	Latitude getLatitude() const;

	// get the coordinate of the junction
	Coordinates getCoordinates() const;

	// print the Junction
	void print() const override;
	
};


