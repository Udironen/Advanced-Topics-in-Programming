#include "IdEntityMap.h"


void IdEntityMap::getWays(std::vector<Way>& ways) {
	for (const auto& pr : id_way_map) {
		ways.push_back(pr.second);
	}
}

void IdEntityMap::getPOIs(std::vector<POI>& pois) {
	for (const auto& pr : id_poi_map) {
		pois.push_back(pr.second);
	}
}
void IdEntityMap::getJunctions(std::vector<Junction>& junctions) {
	for (const auto& pr : id_junction_map) {
		junctions.push_back(pr.second);
	}
}

void IdEntityMap::insert(std::shared_ptr<Entity> ent) {
	if (ent->getType() == Entity::EntityType::Junction) {
		id_junction_map.insert_or_assign( ent->getEntityId() ,*(dynamic_cast<Junction*>(ent.get())) );
	}
	else if (ent->getType() == Entity::EntityType::POI) {
		id_poi_map.insert_or_assign( ent->getEntityId() ,*(dynamic_cast<POI*>(ent.get())) );
	}
	else {
		id_way_map.insert_or_assign( ent->getEntityId() ,*(dynamic_cast<Way*>(ent.get())) );		
	}
}

const std::shared_ptr<Entity> IdEntityMap::findEntity(const EntityId& id) const {
	std::shared_ptr<Entity> p_ent;
	auto junc_itr = id_junction_map.find(id);
	if (junc_itr != id_junction_map.end()) {
		p_ent = std::make_shared<Junction>(junc_itr->second);
		return p_ent;
	}
	auto poi_itr = id_poi_map.find(id);
	if (poi_itr != id_poi_map.end()) {
		p_ent = std::make_shared<POI>(poi_itr->second);
		return p_ent;
	}
	auto way_itr = id_way_map.find(id);
	if (way_itr != id_way_map.end()) {
		p_ent = std::make_shared<Way>(way_itr->second);
		return p_ent;
	}

	return nullptr;
}

std::unordered_map<EntityId, Way, IDhash>::const_iterator IdEntityMap::findWay(const EntityId& WayId) const {
	return id_way_map.find(WayId);
}
std::unordered_map<EntityId, POI, IDhash>::const_iterator IdEntityMap::findPOI(const EntityId& poiId) const {
	return id_poi_map.find(poiId);
}
std::unordered_map<EntityId, Junction, IDhash>::const_iterator IdEntityMap::findJunction(const EntityId& juncId) const {
	return id_junction_map.find(juncId);
}

std::unordered_map<EntityId, Way, IDhash>::const_iterator IdEntityMap::beginWay() const {
	return id_way_map.begin();
}

std::unordered_map<EntityId, Way, IDhash>::const_iterator IdEntityMap::endWay() const {
	return id_way_map.end();
}

std::unordered_map<EntityId, Junction, IDhash>::const_iterator IdEntityMap::endJunction() const {
	return id_junction_map.end();
}

std::unordered_map<EntityId, POI, IDhash>::const_iterator IdEntityMap::endPOI() const{
	return id_poi_map.end();
}

size_t IdEntityMap::size() const {
	size_t size = id_poi_map.size();
	size += id_junction_map.size();
	size += id_way_map.size();
	return size;
}

void IdEntityMap::clear() {
	id_poi_map.clear();
	id_junction_map.clear();
	id_way_map.clear();
}

void IdEntityMap::print() const{
	for (auto const& pr : id_junction_map) {
		std::cout << "{" << (std::string)pr.first << ": ";
		pr.second.print();
		std::cout << " }" << std::endl;
	}
	for (auto const& pr : id_poi_map) {
		std::cout << "{" << (std::string)pr.first << ": ";
		pr.second.print();
		std::cout << " }" << std::endl;
	}
	for (auto const& pr : id_way_map) {
		std::cout << "{" << (std::string)pr.first << ": ";
		pr.second.print();
		std::cout << " }" << std::endl;
	}
}

