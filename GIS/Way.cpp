#include "Way.h"

Way::Way(const EntityId&				id_,
		const std::string&				name_,
		const std::string&				description_,
		const std::set<std::string>&	tags_,
		const EntityId&					from_,
		const EntityId&					to_,
		const std::string&				direction_,
		const int						speed_limit_,
		const bool						toll_road_,
		const std::vector<Coordinates>& curves_,
		const std::vector<std::string>& restricted_)
		:
		Entity(id_, name_, description_, tags_, EntityType::Way),
		from(from_),
		to(to_),
		direction(direction_),
		speed_limit(speed_limit_),
		toll_road(toll_road_),
		length(0),
		from_coords(Longitude(defult_longi), Latitude(defult_lati)),
		to_coords(Longitude(defult_longi), Latitude(defult_lati)),
		curves(curves_),
		restricted(restricted_)
{	
	high_way = (category_tags.find("highway") != category_tags.end());
}

EntityId Way::getFromEntityId() const {
	return from;
}

EntityId Way::getToEntityId() const {
	return to;
}

std::string Way::getDirection() const {
	return direction;
}

int Way::getSpeedLimit() const {
	return speed_limit;
}

bool Way::isToll() const {
	return toll_road;
}

const std::vector<Coordinates>& Way::getCurves() const {
	return curves;
}

const std::vector<std::string>& Way::getRestricted() const {
	return restricted;
}

void Way::print() const {
	std::cout << "way : " << std::endl;
	Entity::print();
	std::cout << (std::string)from << ", ";
	std::cout << (std::string)to << ", ";
	std::cout << direction << ", ";
	std::cout << speed_limit << ", ";
	std::cout << toll_road << std::endl;
	std::cout << "[ " << (double)from_coords.longitude() << " , " << (double)from_coords.latitude() << " ] " << std::endl;
	std::cout << "[ " << (double)to_coords.longitude() << " , " << (double)to_coords.latitude() << " ] " << std::endl;
}

bool Way::isBidirectional() const {
	return (direction.compare("bidirectional") == 0);
}

bool Way::isHighway() const {
	return high_way;
}

std::pair<EntityId, EntityId> Way::getJunctions() const {
	return { from,to };
}

Meters Way::getLength() const {
	return Meters(length);
}

void Way::setLength(Meters length_) {
	length = length_;
}

void Way::addPartialLength(const Meters length) {
	partial_length.push_back(length);
}

Meters Way::getPartialLength(const int index) const {
	return partial_length[index];
}

const Coordinates& Way::getFromJunctionCoordinates() const {
	return from_coords;
}

const Coordinates& Way::getToJunctionCoordinates() const {
	return to_coords;
}

void Way::setFromJunctionCoordinates(const Coordinates& coords) {
	from_coords = coords;
}

void Way::setToJunctionCoordinates(const Coordinates& coords) {
	to_coords = coords;
}

std::pair<Meters, Meters>
Way::getSegmentPartsOnWay(std::size_t segment, const Coordinates& c) const {
	Meters from_length(0);
	if (segment == 0) {
		from_length = CoordinatesMath::calculateDistance(c, from_coords);
	}
	else {
		from_length = CoordinatesMath::calculateDistance(c, curves[segment - 1]);
		from_length += getPartialLength(segment - 1);
	}
	Meters to_length = length - from_length;

	return { from_length,to_length };
}


std::pair<Meters, Meters> Way::distanceFromTo(const Coordinates& coords, const size_t segment) const {
	Meters from_length(0);

	if (segment == 0) {
		from_length = CoordinatesMath::calculateDistance(coords, from_coords);
	}
	else {
		from_length = CoordinatesMath::calculateDistance(coords, curves[segment - 1]);
		from_length += getPartialLength(segment - 1);
	}
	Meters to_length = length - from_length;
	return { from_length,to_length };
}

bool Way::isValidPoint(const Coordinates& point, const Coordinates& coordinate) const {
	Meters distance = CoordinatesMath::calculateDistance(point, coordinate);
	return !((isHighway()) && ((double)distance > 3));
}

