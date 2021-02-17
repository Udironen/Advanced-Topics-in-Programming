#pragma once

#include "Circle.h"
#include <string>
#include <iostream>
#include <vector>

class POI: public Entity{
friend class GIS;
	
	Circle						circle;
	std::vector<std::string>	accessibility;
	
public:
	//	constructors
	POI(const EntityId&					id_,
		const std::string&				name_,
		const std::string&				description_,
	 	const std::set<std::string>&	tags_,
		const Circle&					circle_,
		const std::vector<std::string>& accessibility_);

	~POI() = default;
	
	// get Shape of POI
	const Circle& getShape() const;
	
	// get vector of accessibility flags
	const std::vector<std::string>& getAccessibility() const;
	

	// print POI
	void print() const;

};

