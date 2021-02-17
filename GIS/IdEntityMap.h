#pragma once

#include "Junction.h"
#include "Way.h"
#include "POI.h"
#include "../UserCommon/Hashers.h"
#include<iterator>
#include <memory>
#include <unordered_map>

class IdEntityMap {
	friend class GIS_313452542;
	std::unordered_map<EntityId, Junction, IDhash> id_junction_map;
	std::unordered_map<EntityId, POI, IDhash> id_poi_map;
	std::unordered_map<EntityId, Way, IDhash> id_way_map;
public:

	void getWays(std::vector<Way>& ways);
	void getPOIs(std::vector<POI>& pois);
	void getJunctions(std::vector<Junction>& junctions);

	void insert(std::shared_ptr<Entity> ent);

	const std::shared_ptr<Entity> findEntity(const EntityId& id) const;
	std::unordered_map<EntityId, Way, IDhash>::const_iterator findWay(const EntityId& WayId) const;
	std::unordered_map<EntityId, POI, IDhash>::const_iterator findPOI(const EntityId& poiId) const;
	std::unordered_map<EntityId, Junction, IDhash>::const_iterator findJunction(const EntityId& juncId) const;
	std::unordered_map<EntityId, Way, IDhash>::const_iterator beginWay() const;

	std::unordered_map<EntityId, Way, IDhash>::const_iterator endWay() const;
	std::unordered_map<EntityId, Junction, IDhash>::const_iterator endJunction() const;
	std::unordered_map<EntityId, POI, IDhash>::const_iterator endPOI() const;

	size_t size() const;

	void clear();

	void print() const;
};