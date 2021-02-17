#pragma once

#include "Junction.h"
#include "../Common/CoordinatesMath.h"
#include "../Common/AbstractWay.h"
#include <string>

class Way: public Entity, public AbstractWay{
	friend class GridWay;	

	EntityId from;
	EntityId to;
	std::string direction;
	int speed_limit;
	bool toll_road;
	Meters length;
	Coordinates from_coords;
	Coordinates to_coords;
	bool high_way;
	std::vector<Coordinates> curves;
	std::vector<std::string> restricted;
	std::vector<Meters> partial_length;

public:
	static double constexpr defult_longi = 0;
	static double constexpr defult_lati = 90;
	//	constructors
	Way(const EntityId&					id_,
		const std::string&				name_,
		const std::string&				description_,
		const std::set<std::string>&	tags_,
		const EntityId&					from_,
		const EntityId&					to_,
		const std::string&				direction_,
			  int						speed_limit_,
			  bool						toll_road_,
		const std::vector<Coordinates>&	curves_,
		const std::vector<std::string>&	restricted_);

	Way(const Way& way) = default;
	Way(Way&& way) = default;

	~Way() = default;

	Way& operator=(const Way& other) = default;

	Way& operator=(Way&& other) = default;

	// get the id of from junction
	EntityId getFromEntityId() const;

	// get the id of to junction
	EntityId getToEntityId() const;

	// get direction of way, bidirectional or unidirectional
	std::string getDirection() const;

	// get speed limit of way	
	int getSpeedLimit() const; // as Km/h 
	
	// get toll road of way
	bool isToll() const;
	
	// get curves of way
	const std::vector<Coordinates>& getCurves() const;
	
	// get vector of unauthorized traffic in this way
	const std::vector<std::string>& getRestricted() const;
	
	// print way
	void print() const override;

	//check if way is bidirectional
	bool isBidirectional() const;

	// get pair of id-s {from,to}
	std::pair<EntityId, EntityId> getJunctions() const; 
	
	// get length of way
	Meters getLength() const; 

	//check if way is a highway
	bool isHighway() const;
	
	// set length of way
	void setLength(const Meters length_);

	//set partial_length
	void addPartialLength(const Meters length);
	
	//get partial_length at index 
	Meters getPartialLength(const int index) const;

	// get coordinates of junction from
	const Coordinates& getFromJunctionCoordinates() const;

	// get coordinates of junction to
	const Coordinates& getToJunctionCoordinates() const;

	std::pair<Meters, Meters> // = {from-junction->point, point->to-junction}
		getSegmentPartsOnWay(std::size_t segment, const Coordinates& c) const;

	std::pair<Meters, Meters> distanceFromTo(const Coordinates& coords, const size_t segment) const;

	bool isValidPoint(const Coordinates& point, const Coordinates& coordinate) const;

	void setFromJunctionCoordinates(const Coordinates& coords);

	void setToJunctionCoordinates(const Coordinates& coords);

	// get the id of entity
	const EntityId& getId() const {
		return id;
	}

	

};


