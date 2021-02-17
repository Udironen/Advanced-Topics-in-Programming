#include "POI.h"

POI::POI(	const EntityId&					id_,
			const std::string&				name_,
			const std::string&				description_,
			const std::set<std::string>&	tags_,
			const Circle& circle_,
			const std::vector<std::string>& accessibility_)
			:
			Entity			(id_, name_, description_, tags_, EntityType::POI),
			circle			(circle_),
			accessibility	(accessibility_)	{}


// get Shape of POI
const Circle& POI::getShape() const {
	return circle;
}

const std::vector<std::string>&
POI::getAccessibility() const
{
	return accessibility;
}

void POI::print() const {
	std::cout << "poi : " << std::endl;
	Entity::print();
	circle.print();
}



