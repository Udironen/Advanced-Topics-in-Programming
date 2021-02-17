#pragma once

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "Entity.h"
#include "Junction.h"
#include "Way.h"
#include "POI.h"
#include "GridWay.h"
#include "GridEntity.h"
#include "../Common/GISNamedTypes.h"
#include "../Common/CoordinatesMath.h"
#include "IdEntityMap.h"
#include "../Common/Restrictions.h"
#include "../Common/AbstractGIS.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <utility>
#include <optional>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include "../Common/GISRegistration.h"




using namespace rapidjson;


class GIS_313452542 : public AbstractGIS {
	static constexpr size_t len_rand_str = 10;
	static constexpr size_t num_of_iterations = 10;
	
	//container for std::size_t  loadMapFile(const std::string& filename);
	std::unordered_map<EntityId, Coordinates, IDhash> juncidCoorMap;

	//container for std::size_t saveMapFile(const std::string& filename);
	IdEntityMap idEntityMap;


	//container for getEntities(const std::string& search_name)
	std::unordered_map<std::string, std::vector<EntityId>> nameIDvecMap;//id vector for the whole GIS

	//container for getEntities(const std::string& search_name, 
	//							const Coordinates&, Meters radius)
	GridEntity grid;

	//container for getWayClosestPoint
	GridWay grid_way;

	//container for getWaysByJunction
	std::unordered_map<EntityId, std::unordered_set<EntityId, IDhash>, IDhash> juncId_waysId_map;

	//---------------------------functions for loadMapFile--------:

	//if ent is valid, insert ent to the GIS and return true
	//else, return false
	bool insertEntityToGIS(std::shared_ptr<Entity>& ent);

	//insert pair {name, id} to nameIDvecMap according to the iterator seearch
	void insertToNameIdvecMap(const std::unordered_map<std::string, std::vector<EntityId>>::iterator& search, const std::string& name, const EntityId id);

	//insert {juncID, wayID} to juncId_waysId_map 
	void insertTojuncIdWaysIDMap(const EntityId& juncID, const EntityId& wayID);

	//insert ent to idEntityMap according to ent type
	void insertToIdEntityMap(std::shared_ptr<Entity> ent);

	//generates a random string of length length
	std::string random_string(size_t length) const;

	//loading the GIS with vector of pointers of entities entitys and and updates idVec with the id of the entities 
	void loadEntities(std::vector<std::shared_ptr<Entity>>& entitys, std::vector<EntityId>& idVec);

	//print error msg to screen.
	void printError(std::string error) const;

	//  Parse a JSON text string to a document return 1 if success and else return 0;
	int createDocument(const std::string& filename, rapidjson::Document& document);

	//check validity of entities before inserting to gis
	bool validGeomtry(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validAccessibility(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validTypeGeomtry(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validRadius(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validCircleCoor(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validFrom(Value::ConstValueIterator itr, std::string type, std::string name) const;
	//check validity of entities before inserting to gis
	bool validTo(Value::ConstValueIterator itr, std::string type, std::string name) const;
	//check validity of entities before inserting to gis
	bool validDirection(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validSpeedLimit(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validtollRoad(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validCurves(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validRestricted(Value::ConstValueIterator itr, std::string name) const;
	//check validity of entities before inserting to gis
	bool validCoordinates(Value::ConstValueIterator itr, std::string type, std::string name) const;
	//check validity of entities before inserting to gis
	bool validName(Value::ConstValueIterator itr, std::string type) const;
	//check validity of entities before inserting to gis
	bool validCategoryTag(Value::ConstValueIterator itr, std::string type, std::string name, std::set<std::string>& category_tags) const;
	//check validity of entities before inserting to gis
	bool validDescription(Value::ConstValueIterator itr, std::string type, std::string name, std::string& description) const;
	//check validity of entities before inserting to gis
	bool validId(Value::ConstValueIterator itr, std::string type, std::string name, std::string& id) const;
	//check validity of entities before inserting to gis
	bool validType(Value::ConstValueIterator itr) const;

	// if itr points to a valid junction, insert pointer to junction to entities vector
	//id: junction id
	//name: junction name
	//description: junction description
	//category_tags: junctions category tags
	void insertJunction(std::vector<std::shared_ptr<Entity>>& entities, const Value::ConstValueIterator& itr,
		const std::string& id, const std::string& name,
		const std::string& description, const std::set<std::string>& category_tags);

	// if itr points to a valid way, insert pointer to way to entities vector
	//id: way id
	//name: way name
	//description: way description
	//category_tags: way category tags
	void insertWay(std::vector<std::shared_ptr<Entity>>& entities, const Value::ConstValueIterator& itr,
		const std::string& id, const std::string& name, const std::string& description,
		const std::set<std::string>& category_tags);

	// if itr points to a valid POI, insert pointer to POI to entities vector
	//id: POI id
	//name: POI name
	//description: POI description
	//category_tags: POI category tags
	void insertPOI(std::vector<std::shared_ptr<Entity>>& entities, const Value::ConstValueIterator& itr,
		const std::string& id, const std::string& name, const std::string& description,
		const std::set<std::string>& category_tags);


	//insert all valid entities from the rapidjson::Document document to a vector of pointers of entities
	void createEntityVector(rapidjson::Document& document, std::vector<std::shared_ptr<Entity>>& entities);

	//---------------------------functions for saveMapFile--------:

	//just for get entities from save map function
	void getJunctions(std::vector<Junction>& junctions);
	void getWays(std::vector<Way>& ways);
	void getPOIs(std::vector<POI>& pois);

	//writing to rapidjson file
	void insertKeyString(const char* key, std::string val, PrettyWriter<StringBuffer>& writer);
	//writing to rapidjson file
	void insertSharedField(const std::unique_ptr<Entity> ent, PrettyWriter<StringBuffer>& writer);
	//writing to rapidjson file
	void insertJunctionObject(const Junction& junc, PrettyWriter<StringBuffer>& writer);
	//writing to rapidjson file
	void insertWayObject(const Way& way, PrettyWriter<StringBuffer>& writer);
	//writing to rapidjson file
	void insertPOIObject(const POI& poi, PrettyWriter<StringBuffer>& writer);

	//---------------------------functions for getEntities-----------------------------------------------------------:
		
	//check if point is inside the circle
	bool ispointInCircle(const Circle& circle, const Coordinates& point) const;

	//check if *ent is inside the circle that defined by center and radius
	bool isEntityInCircle(const std::shared_ptr<Entity> p_ent, const Coordinates& center, Meters radius) const;

	//check if *junc is inside the circle that defined by center and radius
	bool isJunctionInCircle(const Junction* junc, const Coordinates& center, Meters radius) const;

	//check if poi is inside the circle that defined by center and radius
	bool isPOIInCircle(const POI& poi, const Coordinates& center, Meters radius) const;

	//check if *way is inside the circle that defined by center and radius
	bool isWayInCircle(const Way* way, const Coordinates& center, Meters radius) const;


	//---------------------------functions for getWayClosestPoint-----------------------------------------------------------:


	//find all the ways-id inside the circle that defined by coordinates and radius and store them in waysId_vec
	//coordinates: the center of the circle we are searching in
	//radius: the radius of the circle we are searching in
	//waysId_vec: the vector of id-s of ways in the circle, will be updated with the relevant id-s
	void getWays(const Coordinates& coordinates, const Meters radius, std::vector<EntityId>& waysId_vec) const;

	//check if id is the EntityId of a way that is closer to coordinates than the current closest_id.
	//if so, it will update closest_coor, closest_id, closest_dist.
	void checkIfCloser(const Coordinates& coordinates, const EntityId& id,
		std::pair<Coordinates, std::size_t>& pair_coords_segment,
		EntityId& closest_id, Meters& closest_dist) const;

	//return true if way_id is a EntityId of a way that does not fall to the restrictions of res.
	bool isWayNOTRestricted(const EntityId& way_id, const Restrictions& res) const;

	//return true if way_id is a EntityId of a way that fall to the restrictions of res.
	bool isWayRestricted(const EntityId& way_id, const Restrictions& res) const;

	//remove all the restricted ways
	void removeRestrictedWays(std::vector<EntityId>& waysId, const Restrictions& res) const;


	//---------------------------functions for getEntityClosestPoint-----------------------------------------------------------:


		//the function shall get *entity and Coordinates		
			//return the closest Coordinates, on entity’s geometry, to the provided Coordinates.
	Coordinates getEntityClosestPoint(const std::shared_ptr<Entity> ent, const Coordinates coordinats)const;

	//the function shall get POI and Coordinates		
		//return the closest Coordinates, on POI’s geometry, to the provided Coordinates.
	Coordinates getPOIClosestPoint(const POI& poi, const Coordinates& coordinats) const;

	//the function shall get Way and Coordinates		
		//return the closest Coordinates, on way, and segment, to the provided Coordinates.
	std::pair<Coordinates, std::size_t>
		getWayClosestPoint(const Way& way, const Coordinates& coordinats) const;


	//print EntityIDMap
	//void printEntityIDMap() const;

	//print NameIdVecMap
	void printNameIdVecMap() const;

	//print grid
	void printGrid() const;
	// Privte field

	//prints all containers
	void print() const;//testing

	//print ent according to its type
	void printEnt(Entity* ent) const;//testing


public:

	~GIS_313452542() = default;

	void printEntityIDMap() const;

	// the function clears all data from the system and returns the number of entities that were cleared
	std::size_t clear();


	//the function loads a map file and returns a vector of all the EntityIds that were loaded (either Ids that 
	//appeared in file or generated) in the exact order as the objects that appeared in the file
	//- in case the system had existing data, the data loaded from file is ​added​ to the existing data
	//- in case entityId from file exists already the old data related to this id is ​replaced​ with the new data
	//- in case an entity in file doesn’t have an Id, a new Id would be generated for it automatically
	std::vector<EntityId> loadMapFile(const std::string& filename);


	//the function saves all data into a map file and returns the number of entities that were saved,
	//the saved entities must include their Ids
	std::size_t saveMapFile(const std::string& filename);


	//  the function returns a vector of EntityIds for all entities that match the search_name
	//  all exact matches (case sensitive) shall be first in the vector
	//  right after shall ​optionally​ appear “partial matches”, in any order (e.g. case insensitive match, matching
	//  based on the longer description, match based on the category tags etc. partial match may work
	//  differently when the search is coordinates based or not - note: partial match implementation is not
	//  mandatory, however it may grant you a bonus.
	//  To qualify for bonus you must include in your submission zip a folder named ​bonus.partial_search,​
	//  that contains ​three​ items:
	//   - partial_search.txt: ​explaining what you did and why qualify for bonus - include where code handels this exceptional partial search
	//   - mapFile.json​ with 10-20 entities
	//   - A file named: partial_search_test​.cpp ​that includes a google test that ​passes​ and
	//     demonstrates your exceptional search
	std::vector<EntityId> getEntities(const std::string& search_name)const;


	//   same as above, but restricted to a search area of the given circle
	std::vector<EntityId> getEntities(const std::string& search_name,
		const Coordinates& coordinates, Meters radius);


	//the function shall get Coordinates and return the closest Coordinates of
	//a Way (the Coordinates may be a point on a the way that is closest to the
	//given Coordinates) and the EntityId of this way, and the segment
	std::tuple<Coordinates, EntityId, std::size_t> getWayClosestPoint(
		const Coordinates& coordinates) const;

	//The function shall get EntityId of a Junctionand return a vector of Ids
	//of all the ways that start at this junction and those which end at this junction
	//and are bidirectional.
	//If the given Id is not known or is not of a junction the function shall return an empty vector
	std::vector<EntityId> getWaysByJunction(const EntityId& junctionId) const;

	//The function shall get EntityId of a Way and return the Way itself.
	//In case the id does not represent a valid way throw an exception.

	//const Way& getWay(const EntityId& wayId) const;

	const AbstractWay& getWay(const EntityId& way_id) const;

	//The function shall get Coordinatesand Restrictions and return the closest Coordinates
	//along a Way that complies to the restrictions provided and the EntityId of this way, and the segment.
	std::tuple<Coordinates, EntityId, std::size_t> getWayClosestPoint(
		const Coordinates&, const Restrictions& res) const;


	//the function shall get EntityId and Coordinates
	//	if the Id is not known, the function returns an empty optional
	//		otherwise, return the closest Coordinates, on entity’s perimeter, to the provided Coordinates.
	//		(the Coordinates shall be a point on a the entity’s geometry that is closest to the given Coordinates)
	std::optional<Coordinates> getEntityClosestPoint(	const		EntityId& id,
														const		Coordinates& coordinates) const;

};
