#include "Junction.h"

Junction::Junction(	const EntityId&					id_,
					const std::string&				name_,
					const std::string&				description_,
					const std::set<std::string>&	tags_,
					const Coordinates& coor_)
					:
					Entity(	id_,
							name_,
							description_,
							tags_,
							EntityType::Junction),
					
					coor(coor_.longitude(), coor_.latitude()) {}

Longitude Junction::getLongitude() const{
	return coor.longitude();
}
Latitude Junction::getLatitude() const{
	return coor.latitude();
}
Coordinates Junction::getCoordinates() const{
	return coor;
}

	//print junction field
void Junction::print() const{
	std::cout << "junction : " << std::endl;

	Entity::print();
	Entity::printCoordinates(coor);

}
