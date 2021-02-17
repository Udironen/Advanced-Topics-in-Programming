GIS

The GIS data is based on:
-	Coordinates: described by Longitude and Latitude
-	Point of Interest
-	Junctions and roads, represented as coordinates connected by ways/arcs
-	A separation of data into blocks or cells, to allow quick search of given coordinates (which road I’m on? what is near me?) without the need to search the entire map

#########################################################################
Features:

----------------------------------------------------------
The GIS can load a Json file by the following structure:
[
  {
    "type": "POI",
    "id": "P1001",
    "name": "Washington Square Park",
    "description": "To circle the square",
    "category_tags": [ "" ],
    "geometry": {
      "type": "Circle",
      "coordinates": [ 40.731051, -73.999611 ],
      "radius": 100000.2
    }
  },
  
  {
    "type": "Junction",
    "id": "J1001",
    "name": "5th Ave & Washington Square N",
    "description": ".*",
    "category_tags": [],
    "coordinates": [ 37.731437, -73.996967 ]
  },

  {
    "type": "Junction",
    "id": "J1002",
    "name": "5th Ave & 8th St",
    "description": ".*",
    "category_tags": [],
    "coordinates": [ 40.732254, -73.996394 ]
  },  
  {
    "type": "Way",
    "id": "W2001",
    "name": "5th Ave btw. Washington Square N & 8th St",
    "description": ".*",
    "category_tags": [],
    "from": {
      "entity_id": "J1002"
    },
    "to": {
      "entity_id": "J1001"
    },
    "curves": [ [ 1, 0 ] ],
    "direction": "unidirectional",
    "speed_limit": 40,
    "toll_road": false,
    "restricted": [ "trucks" ]
  },
  
]
---------------------------------------------------------------

The GIS can save the data it contains to a Json file with the same structure.
--------------------------------------------------------------------------------
The GIS can search entities by name, and also in a restricted erea.
----------------------------------------------------------------------------
The GIS can find the closest way to a specific coordinates, and the closest coordinates on it.
------------------------------------------------------------------------------------------------
The GIS can find the closest point on an entity of the GIS to another coordinates.
--------------------------------------------------------------------------------------------------


EX2 Features:

The GIS can find the shortest route by distance and by time, given start and end points. the pointrs don't have to be on a certain way! 

####################################################################################################################

API for searching data

Types
template<typename T>
class NamedType {
	T val;
public:
	explicit NamedType(const T& t): val(t) {}
	operator const T&() const {
		return val;
}
};

struct Longitude: NamedType<double> { using NamedType<double>::NamedType;  };
struct Latitude: NamedType<double> { using NamedType<double>::NamedType; };
struct Meters: NamedType<double> { using NamedType<double>::NamedType; };

strt EntityId: NamedType<std::string> { 
  using NamedType<std::string>::NamedType;
};

class Coordinates {
       Longitude _longitude;
       Latitude _latitude;
public:
       Coordinates(Longitude longitude, Latitude latitude) : _longitude(longitude), _latitude(latitude) {}
       Longitude longitude() const { return _longitude; }
       Latitude latitude () const { return _latitude; }
       bool operator==(const Coordinates& rhs) const {
               return _longitude == rhs._longitude && _latitude == rhs._latitude;
       }
};

#########################################################################################################################################################################################################################

GIS Public Functions:

std::size_t GIS::clear();
the function clears all data from the system and returns the number of entities that were cleared

std::vector<EntityId> GIS::loadMapFile(const std::string& filename);
the function loads a map file and returns a vector of all the EntityIds that were loaded (either Ids that appeared in file or generated) in the exact order as the objects that appeared in the file
- in case the system had existing data, the data loaded from file is added to the existing data
- in case an entity in file doesn’t have an Id, a new Id would be generated for it automatically
- in case entityId from file exists already the old data related to this id is replaced with the new data
- in case an entity is not in the right format, it will not be loaded. it will be noted in errors.log file.

std::size_t GIS::saveMapFile(const std::string& filename);
the function saves all data into a map file and returns the number of entities that were saved, the saved entities must include their Ids

std::vector<EntityId> GIS::getEntities(const std::string& search_name);
the function returns a vector of EntityIds for all entities that match the search_name
all exact matches (case sensitive) shall be first in the vector
right after shall optionally appear “partial matches”, in any order (e.g. case insensitive match, matching based on the longer description, match based on the category tags etc. partial match may work differently when the search is coordinates based or not  - note: partial match implementation is not mandatory, however it may grant you a bonus. 
To qualify for bonus you must include in your submission zip a folder named bonus.partial_search, that contains three items: 
-	partial_search.txt: explaining what you did and why qualify for bonus - include where code handels this exceptional partial search
-	mapFile.json with 10-20 entities
-	A file named: partial_search_test.cpp that includes a google test that passes and demonstrates your exceptional search

std::vector<EntityId> GIS::getEntities(const std::string& search_name, const Coordinates&, Meters radius);
	same as above, but restricted to a search area of the given circle

std::optional<Coordinates> GIS::getEntityClosestPoint(const EntityId&, const Coordinates&);
the function shall get EntityId and Coordinates
if the Id is not known, the function returns an empty optional
otherwise, return the closest Coordinates, on entity’s perimeter, to the provided Coordinates.
(the Coordinates shall be a point on a the entity’s geometry that is closest to the given Coordinates)

std::pair<Coordinates, EntityId> GIS::getWayClosestPoint(const Coordinates&);
the function shall get Coordinates and return the closest Coordinates of a Way (the Coordinates may be a point on a the way that is closest to the given Coordinates) and the EntityId of this way

//The function shall get EntityId of a Junctionand return a vector of Ids
//of all the ways that start at this junction and those which end at this junction
//and are bidirectional.
//If the given Id is not known or is not of a junction the function shall return an empty vector
std::vector<EntityId> getWaysByJunction(const EntityId& junctionId) const;

//The function shall get EntityId of a Way and return the Way itself.
//In case the id does not represent a valid way throw an exception.
const Way& getWay(const EntityId& wayId) const;

//The function shall get Coordinatesand Restrictions and return the closest Coordinates
//along a Way that complies to the restrictions provided and the EntityId of this way.
std::pair<Coordinates, EntityId> getWayClosestPoint(
	const Coordinates&, const Restrictions& res) const;
################################################################################################################################################################################################################
Navigation Public Functions:    

public:
    //constractor
    Navigation(const NavigationGIS& gis_); 
    
  //  method gets a start point(that might be on a way or not) and an end point.The return value is a Routes object holding shortest by distance and shortest by time Route objects, each with the following data :
  //  -A vector of all ways to travel on, in proper order.
  //  - The start point on the first way closest to the source - this point must be a valid point on the first way in the vector(index 0) .
  //  - The end point on the last way closest to the destination - this point must be a valid point on the last way in the vector.
  //  - Distance and duration of the route(addition 15.12: from start point to end point as described above).
    Routes getRoutes(const Coordinates& start, const Coordinates& end) const;
    
};


################################################################################################################################################################################################################


GIS Private Fields and Expressions:


	//container for std::size_t  loadMapFile(const std::string& filename);
	std::unordered_map<EntityId, Coordinates, IDhash> juncidCoorMap;
		IDhash is a Hash for EntityId

	//container for std::size_t saveMapFile(const std::string& filename);
	std::unordered_map<EntityId, Entity*, IDhash> idEntityMap;

	//container for getEntities(const std::string& search_name)
	std::unordered_map<std::string, std::vector<EntityId>> nameIDvecMap;//id vector for the whole GIS

	//container for getEntities(const std::string& search_name, 
	//										const Coordinates&, Meters radius)
	Grid grid;

	//container for getWayClosestPoint
	GridWay grid_way;

######################################################################################################################################################################################################################

GIS Private Functions:

//---------------------------functions for loadMapFile--------:

	//insert ent to the GIS
	void insertEntityToGIS(Entity* ent);

	//insert pair {name, id} to nameIDvecMap according to the iterator seearch
	void insertToNameIdvecMap(const std::unordered_map<std::string, std::vector<EntityId>>::iterator& search, const std::string& name, const EntityId id);

	//insert &ent to idEntityMap according to ent type
	void insertToIdEntityMap(Entity* ent);

	//generates a random string of length length
	std::string random_string(size_t length);

	//loading the GIS with vector of pointers of entities entitys and and updates idVec with the id of the entities 
	void loadEntities(const std::vector<Entity*>& entitys, std::vector<EntityId>& idVec);
	
	//print error msg to screen. need modify to error.log //////////
	void printError(std::string error);

	//  Parse a JSON text string to a document return 1 if success and else return 0;
	int createDocument(const std::string& filename, rapidjson::Document& document);

	//check validity of entities before inserting to gis
	bool validGeomtry(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validAccessibility(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validTypeGeomtry(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validRadius(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validCircleCoor(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validFrom(Value::ConstValueIterator itr, std::string type, std::string name);
	//check validity of entities before inserting to gis
	bool validTo(Value::ConstValueIterator itr, std::string type, std::string name);
	//check validity of entities before inserting to gis
	bool validDirection(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validSpeedLimit(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validtollRoad(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validCurves(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validRestricted(Value::ConstValueIterator itr, std::string name);
	//check validity of entities before inserting to gis
	bool validCoordinates(Value::ConstValueIterator itr, std::string type, std::string name);
	//check validity of entities before inserting to gis
	bool validName(Value::ConstValueIterator itr, std::string type);
	//check validity of entities before inserting to gis
	bool validCategoryTag(Value::ConstValueIterator itr, std::string type, std::string name, std::vector<std::string>& category_tags);
	//check validity of entities before inserting to gis
	bool validDescription(Value::ConstValueIterator itr, std::string type, std::string name, std::string& description);
	//check validity of entities before inserting to gis
	bool validId(Value::ConstValueIterator itr, std::string type, std::string name, std::string& id);
	//check validity of entities before inserting to gis
	bool validType(Value::ConstValueIterator itr);

	// if itr points to a valid junction, insert pointer to junction to entities vector
	//id: junction id
	//name: junction name
	//description: junction description
	//category_tags: junctions category tags
	void insertJunction(std::vector<Entity*>& entities, const Value::ConstValueIterator& itr,
	 						const std::string id, const std::string name, 
	 						const std::string description, const std::vector<std::string> category_tags);
	
	// if itr points to a valid way, insert pointer to way to entities vector
	//id: way id
	//name: way name
	//description: way description
	//category_tags: way category tags
	void insertWay(std::vector<Entity*>& entities, const Value::ConstValueIterator& itr,
					const std::string id, const std::string name, const std::string description, 
					const std::vector<std::string> category_tags);

	// if itr points to a valid POI, insert pointer to POI to entities vector
	//id: POI id
	//name: POI name
	//description: POI description
	//category_tags: POI category tags
	void insertPOI(std::vector<Entity*>& entities, const Value::ConstValueIterator& itr, 
					const std::string id, const std::string name, const std::string description, 
					const std::vector<std::string> category_tags);


	//insert all valid entities from the rapidjson::Document document to a vector of pointers of entities
	void createEntityVector(rapidjson::Document& document, std::vector<Entity*>& entities);

	//---------------------------functions for saveMapFile--------:

	//just for get entities from save map function
	std::vector<Entity*> getEntitiesVector() const;

	//writing to rapidjson file
	void insertKeyString(const char* key, std::string val, PrettyWriter<StringBuffer>& writer);
	//writing to rapidjson file
	void insertSharedField(Entity* ent, PrettyWriter<StringBuffer>& writer);
	//writing to rapidjson file
	void insertJunctionObject(PrettyWriter<StringBuffer>& writer, Junction*& pJ, Entity* ent);
	//writing to rapidjson file
	void insertWayObject(PrettyWriter<StringBuffer>& writer, Way*& pW, Entity* ent);
	//writing to rapidjson file
	void insertPOIObject(PrettyWriter<StringBuffer>& writer, POI*& pP, Entity* ent);

	//---------------------------functions for getEntities-----------------------------------------------------------:

	//updating iter_longi to the next iterator in cyclic order
	void getNextLongiIter(Longi_iter& iter_longi, Longi_iter& begin_of_lati, Longi_iter& end_of_lati);

	//updating iter_longi to the previous iterator in cyclic order
	void getPrevLongiIter(Longi_iter& iter_longi, Longi_iter& begin_of_lati, Longi_iter& end_of_lati);

	//updating lati_iter to the previous iterator in cyclic order
	void getPrevLatiIter(Lati_iter& lati_iter, bool& is_out_of_sphere);
	
	//updating lati_iter to the next iterator in cyclic order
	void getNextLatiIter(Lati_iter& lati_iter, bool& is_out_of_sphere);

	//updating iter to be iterator that points to grid[grid_cell.latitude()]
	//if GridCell grid_cell does not exist in the grid, insert the GridCell
	bool getCellIter(Lati_iter& iter, const GridCell& grid_cell);

	//updating iter to be iterator that points to grid[grid_cell.latitude()][grid_cell.longitude()]
	//if GridCell grid_cell does not exist in the grid, insert the GridCell
	bool getCellIter(Longi_iter& iter, const GridCell& grid_cell);


	//searching in the top half of the circle that defined by coordinates and radius 
	//iter_lati: iterator to the first GridCell in the grid to begin the search
	//starting_cell: the GridCell in the grid to begin the search
	//search_name: the name we are searching
	//coordinates: the center of the circle we are searching in
	//radius: the radius of the circle we are searching in
	//id_vector: the vector of id-s of entities with the same name as search_name in the circle, will be updated with the relevant id-s
	//id_set: a set of id-s of entities with the same name as search_name we have already found (might not be in id_vector),
	//will be updated with the id-s that we find for the first time 
	void searchInTop(Lati_iter& iter_lati, const GridCell& starting_cell, const std::string& search_name,
		const Coordinates& coordinates, const Meters& radius, std::vector<EntityId>& id_vector,
		std::unordered_set<EntityId>& id_set);	
	
	//searching in the bottom half of the circle that defined by coordinates and radius 
	//iter_lati: iterator to the first GridCell in the grid to begin the search
	//starting_cell: the GridCell in the grid to begin the search
	//search_name: the name we are searching
	//coordinates: the center of the circle we are searching in
	//radius: the radius of the circle we are searching in
	//id_vector: the vector of id-s of entities with the same name as search_name in the circle, will be updated with the relevant id-s
	//id_set: a set of id-s of entities with the same name as search_name we have already found (might not be in id_vector),
	//		will be updated with the id-s that we find for the first time
	void searchInBottom(Lati_iter& iter_lati, const GridCell& starting_cell, const std::string& search_name,
		const Coordinates& coordinates, const Meters& radius, std::vector<EntityId>& id_vector,
		std::unordered_set<EntityId>& id_set);
	
	//searching in the cells which in grid[starting_cell.latitude()],
	//in the circle that defined by coordinates and radius 
	//starting_cell: the GridCell in the grid to begin the search
	//search_name: the name we are searching
	//coordinates: the center of the circle we are searching in
	//radius: the radius of the circle we are searching in
	//id_vector: the vector of id-s of entities with the same name as search_name in the circle, will be updated with the relevant id-s
	//id_set: a set of id-s of entities with the same name as search_name we have already found (might not be in id_vector),
	//will be updated with the id-s that we find for the first time
	//end_of_lati: iterator to the last pair in grid[starting_cell.latitude()]
	//begin_of_lati: iterator to the first pair in grid[starting_cell.latitude()]
	bool searchInLati(const GridCell& starting_cell, const std::string& search_name, const Coordinates& coordinates,
		const Meters& radius, std::vector<EntityId>& id_vector, std::unordered_set<EntityId>& id_set,
		Longi_iter end_of_lati, Longi_iter begin_of_lati);


	//searching in name_set if an id belongs to an entity that inside
	//the circle that defined by coordinates and radius 
	//name_set: the set we are searching in
	//coordinates: the center of the circle we are searching in
	//radius: the radius of the circle we are searching in
	//id_vector: the vector of id-s of entities that inside the circle, will be updated with the relevant id-s
	//id_set: a set of id-s of entities we have already found (might not be in id_vector),
	//will be updated with the id-s that we find for the first time
	void searchInCell(std::unordered_set<EntityId>& name_set, const Coordinates& coordinates, const Meters& radius,
		std::vector<EntityId>& id_vector, std::unordered_set<EntityId>& id_set);

	//check if point is inside the circle
	bool ispointInCircle(const Circle& circle, const Coordinates& point) const;

	//check if *ent is inside the circle that defined by center and radius
	bool isEntityInCircle(Entity* ent, const Coordinates& center, const Meters radius) const;
	
	//check if *junc is inside the circle that defined by center and radius
	bool isJunctionInCircle(const Junction* junc, const Coordinates& center, const Meters radius) const;

	//check if poi is inside the circle that defined by center and radius
	bool isPOIInCircle(POI& poi, const Coordinates& center, const Meters radius) const;

	//check if *way is inside the circle that defined by center and radius
	bool isWayInCircle(const Way* way, const Coordinates& center, const Meters radius) const;


	//---------------------------functions for getWayClosestPoint-----------------------------------------------------------:


	//find all the ways-id inside the circle that defined by coordinates and radius and store them in waysId_vec
	//coordinates: the center of the circle we are searching in
	//radius: the radius of the circle we are searching in
	//waysId_vec: the vector of id-s of ways in the circle, will be updated with the relevant id-s
	void getWays(const Coordinates& coordinates, Meters radius, std::vector<EntityId>& waysId_vec);

	//searching in the top half of the circle that defined by starting_cell and radius 
	//iter_lati: iterator to the first GridCell in the grid_way to begin the search
	//starting_cell: the GridCell in the grid_way to begin the search
	//radius: the radius of the circle we are searching in
	//id_vector:the vector of id-s of ways in the circle, will be updated with the relevant id-s
	//memory_set: a set of id-s of ways we have already found (might not be in id_vector),
	//will be updated with the id-s that we find for the first time 
	void searchWayInTop(Lati_iter_way& iter_lati, const GridCell& starting_cell,
		const Meters& radius, std::vector<EntityId>& id_vector,
		std::unordered_set<EntityId>& memory_set);

	//searching in the bottom half of the circle that defined by starting_cell and radius 
	//iter_lati: iterator to the first GridCell in the grid_way to begin the search
	//starting_cell: the GridCell in the grid_way to begin the search
	//radius: the radius of the circle we are searching in
	//id_vector:the vector of id-s of ways in the circle, will be updated with the relevant id-s
	//memory_set: a set of id-s of ways we have already found (might not be in id_vector),
	//	will be updated with the id-s that we find for the first time 
	void searchWayInBottom(Lati_iter_way& iter_lati, const GridCell& starting_cell,
		const Meters& radius, std::vector<EntityId>& id_vector,
		std::unordered_set<EntityId>& memory_set);

	//searching in the cells which in grid_way[starting_cell.latitude()],
	//	in the circle that defined by starting_cell and radius 	
	//starting_cell: the GridCell in the grid_way to begin the search	
	//radius: the radius of the circle we are searching in
	//id_vector:the vector of id-s of ways in the circle, will be updated with the relevant id-s
	//id_set: a set of id-s of ways we have already found (might not be in id_vector),
	//	will be updated with the id-s that we find for the first time 
	//end_of_lati: iterator to the last pair in grid_way[starting_cell.latitude()]
	//begin_of_lati: iterator to the first pair in grid_way[starting_cell.latitude()]
	bool searchWayInLati(const GridCell& starting_cell, const Meters& radius, std::vector<EntityId>& id_vector,
		std::unordered_set<EntityId>& id_set, Longi_iter_way end_of_lati, Longi_iter_way begin_of_lati);

	//pushing to id_vector all the id-s that in id_set which are not in memory_set	
	void searchWayInCell(std::unordered_set<EntityId>& id_set,
		std::vector<EntityId>& id_vector, std::unordered_set<EntityId>& memory_set);
	
	//updating iter to be iterator that points to grid_way[grid_cell.latitude()]
	//if GridCell grid_cell does not exist in the grid_way, insert the GridCell
	bool getCellIterWay(Lati_iter_way& iter, const GridCell& grid_cell);

	//updating iter to be iterator that points to grid_way[grid_cell.latitude()][grid_cell.longitude()]
	//if GridCell grid_cell does not exist in the grid_way, insert the GridCell
	bool getCellIterWay(Longi_iter_way& iter, const GridCell& grid_cell);
	
	//updating lati_iter to the next iterator in cyclic order
	void getNextLatiIterWay(Lati_iter_way& lati_iter, bool& is_out_of_sphere);
	
	//updating lati_iter to the previous iterator in cyclic order
	void getPrevLatiIterWay(Lati_iter_way& lati_iter, bool& is_out_of_sphere);

	//updating iter_longi to the next iterator in cyclic order
	void getNextLongiIterWay(Longi_iter_way& iter_longi, Longi_iter_way& begin_of_lati, Longi_iter_way& end_of_lati);
	
	//updating iter_longi to the previous iterator in cyclic order
	void getPrevLongiIterWay(Longi_iter_way& iter_longi, Longi_iter_way& begin_of_lati, Longi_iter_way& end_of_lati);

	
//---------------------------functions for getEntityClosestPoint-----------------------------------------------------------:

	//the function shall get *entity and Coordinates		
		//return the closest Coordinates, on entity’s geometry, to the provided Coordinates.
	Coordinates getEntityClosestPoint( Entity* ent, const Coordinates coordinats)const ; 
	
	//the function shall get POI and Coordinates		
		//return the closest Coordinates, on POI’s geometry, to the provided Coordinates.
	Coordinates getPOIClosestPoint( POI& poi, const Coordinates& coordinats) const;
	
	//the function shall get Way and Coordinates		
		//return the closest Coordinates, on way, to the provided Coordinates.
	Coordinates getWayClosestPoint(const Way& way, const Coordinates& coordinats) const;

	

	//print EntityIDMap
	void printEntityIDMap() const;
	
	//print NameIdVecMap
	void printNameIdVecMap() const;
	
	//print grid
	void printGrid() const;
	// Privte field

	//prints all containers
	void print() const;//testing

	//print ent according to its type
	void printEnt(Entity* ent) const;//testing


################################################################################################################
Navigation Private Functions:

//the algorithm to compute the best route:
    //by distance if is_time = false, 
    //by time if is_time = true.
    //goal: the destination node.
    //seen_node: map of <node_id, Node> of nodes that already have been found.
    //frontier: the priority queue.
    //last_way_id: the id of the way that goal is on it.    
    void aStarSearch(Node& goal,
        std::unordered_map<EntityId, Node, IDhash>& seen_nodes,
        PriorityQueue<iter_node, double>& frontier,
        const EntityId& last_way_id, const bool is_time = false) const;

    //resets node priority  
    void resetNode(Node& node)const;

    //construct path by goal parents
    void constructPath(std::vector<std::pair<EntityId, Direction>>& all_ways, const Node& goal) const;

    //update the neighbors of the node by the aStar algorithm
    void updateNeighbors(const Node& node,
        std::unordered_map<EntityId, Node, IDhash>& seen_nodes,
        PriorityQueue<iter_node, double>& frontier,
        const EntityId& last_way_id, const Node& goal, const bool is_time = false) const;

    //return the correct direction according to from_to and junc_id
    Direction getDirection(
        const std::pair<EntityId, EntityId>& from_to, const EntityId& junc_id) const;

    //return the correct EntityId of the current neighbor according to from_to and junc_id
    const EntityId& getNeighborId(
        const std::pair<EntityId, EntityId>& from_to, const Direction direction) const;

    //return the correct Coordinates of the current neighbor according to from_to and junc_id
    Coordinates getNeighborCoords(
        const Way& way, const Direction direction) const;

    //return true  if start_coor_id and end_coor_id on the same way
    bool isSameWay(
        const std::pair<Coordinates, EntityId>& start_coor_id,
        const std::pair<Coordinates, EntityId>& end_coor_id) const;

################################################################################################################

Assisting Classes:

-	Entity, described by:
#	[optional in loaded files / mandatory in saved files] id: unique across all type of entities (string)
#	name: a short descriptive name (string)
#	[optional] description: longer description (string)
#	[optional] category_tags: a list of tags (strings) - may be empty
	all functions and constructors are mentioned in the documentation of ths code


-	POI (point of interest), inheritates from Entity, described by:
#	all Entity fields
#	[optional] accessibility: may be tagged with accessibility flags (“by car”, “by foot”, “by wheelchair”)
#	geometry: 
		Circle: 
			coordinates: as center of circle 
			Radius: in meters
	all functions and constructors are mentioned in the documentation of ths code


-	Junction, inheritates from Entity, described by:
#	all Entity fields
#	coordinates
	all functions and constructors are mentioned in the documentation of ths code


-	Way, inheritates from Entity, described by:
#	all Entity fields
#	from: EntityId of a junction 
#	to: EntityId of a junction 
#	[optional]: curves: coordinates setting the Way’s layout, in order (from => to)
#	direction: “bidirectional” (two-ways) OR “unidirectional” (from => to)  
#	speed_limit: in km/h
#	toll_road: true / false
#	Coordinates from_coords;
#	Coordinates to_coords;
	all functions and constructors are mentioned in the documentation of ths code


-	Shape, an abstract class


-	Circle, inheritates from Shape, described by:
#	Meters radius
#	Coordinates coordinates;
	all functions and constructors are mentioned in the documentation of ths code


-	Cell, described by:
#	nameIdMap - a map with Key = string (name of entity), Val = set of EntityId
	all functions and constructors are mentioned in the documentation of ths code


-	CellWay, described by:
#	id_set - a set of EntityId
	all functions and constructors are mentioned in the documentation of ths code


-	Grid, described by:
#	std::map<Latitude, std::map<Longitude,Cell,LongiCompare>, LatiCompare> grid
		LongiCompare and LatiCompare are comperators of Longitude and Latitude, in that order.
	all functions and constructors are mentioned in the documentation of ths code

-	GridWay, described by:
#	std::map<Latitude, std::map<Longitude, CellWay, LongiCompare>, LatiCompare> grid
		LongiCompare and LatiCompare are comperators of Longitude and Latitude, in that order.
	all functions and constructors are mentioned in the documentation of ths code


-	CoordinatesMath:
	A class with static functions to make calculations on coordinates
	all functions and constructors are mentioned in the documentation of ths code


-	Double:
	A generic class of rounded double by size_t Presision
	all functions and constructors are mentioned in the documentation of ths code

Assisting Classes for EX2:

-	Node:
#	EntityId id
#	std::shared_ptr<Node> parent
#	EntityId way_from_parent_id
#	Direction direction
#	Meters length_so_far
#	Minutes time_so_far
#	double heuristic
#	Coordinates coords
all functions and constructors are mentioned in the documentation of ths code

-	Route:
#	std::vector<std::pair<EntityId, Direction>> all_ways
#   Coordinates start_point
#   Coordinates end_point
#   Meters total_length
#   Minutes est_duration

- Routes:
 #   Route shortest_distance;
 #   Route shortest_time;
 #   bool is_valid;

