
# Advanced-Topics-in-Programming-GIS

The GIS data is based on:
-	Coordinates: described by Longitude and Latitude
-	Point of Interest
-	Junctions and roads, represented as coordinates connected by ways/arcs
-	A separation of data into blocks or cells, to allow quick search of given coordinates (which road I’m on? what is near me?) without the need to search the entire map


Features:
-
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


The GIS can save the data it contains to a Json file with the same structure.
The GIS can search entities by name, and also in a restricted erea.
The GIS can find the closest way to a specific coordinates, and the closest coordinates on it.
The GIS can find the closest point on an entity of the GIS to another coordinates.
	


API for searching data
-	
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


GIS Public Functions:
-
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
	
std::vector<EntityId> getWaysByJunction(const EntityId& junctionId) const;
	
	The function shall get EntityId of a Junctionand return a vector of Ids
	of all the ways that start at this junction and those which end at this junction
	and are bidirectional.
	If the given Id is not known or is not of a junction the function shall return an empty vector

const Way& getWay(const EntityId& wayId) const;

	The function shall get EntityId of a Way and return the Way itself.
	In case the id does not represent a valid way throw an exception.

std::pair<Coordinates, EntityId> getWayClosestPoint(const Coordinates&, const Restrictions& res) const;

	The function shall get Coordinatesand Restrictions and return the closest Coordinates
	along a Way that complies to the restrictions provided and the EntityId of this way.


Assisting Mendatory Classes:
-
Entity, described by:
	
	[optional in loaded files / mandatory in saved files] id: unique across all type of entities (string)
	name: a short descriptive name (string)
	[optional] description: longer description (string)
	[optional] category_tags: a list of tags (strings) - may be empty

POI (point of interest), inheritates from Entity, described by:
	
	all Entity fields
	[optional] accessibility: may be tagged with accessibility flags (“by car”, “by foot”, “by wheelchair”)
	geometry: 
		Circle: 
			coordinates: as center of circle 
			Radius: in meters

Junction, inheritates from Entity, described by:
	
	all Entity fields
	coordinates

Way, inheritates from Entity, described by:
	
	all Entity fields
	from: EntityId of a junction 
	to: EntityId of a junction 
	[optional]: curves: coordinates setting the Way’s layout, in order (from => to)
	direction: “bidirectional” (two-ways) OR “unidirectional” (from => to)  
	speed_limit: in km/h
	toll_road: true / false
	Coordinates from_coords;
	Coordinates to_coords;

Circle, described by:
	
	Meters radius
	Coordinates coordinates;
	all functions and constructors are mentioned in the documentation of ths code


# Advanced-Topics-in-Programming-Navigation

Navigation Features:
-

The Navigation System, based on the GIS that provided to it, can find the shortest route by distance and by time, given start and end points (Coordinates). the pointrs don't have to be on a certain way.
The Navigation algorithm is based on the A-Star algorithm (which based on Dijkstra, whith modifications to boost the time complexity).


Navigation Public Functions:
-   

Navigation(const NavigationGIS& gis_); 

	constractor

Routes getRoutes(const Coordinates& start, const Coordinates& end) const;
    
	method gets a start point(that might be on a way or not) and an end point.The return value is a Routes object holding shortest by distance and shortest by time Route 	objects, each with the following data :
   	-A vector of all ways to travel on, in proper order.
    	- The start point on the first way closest to the source - this point must be a valid point on the first way in the vector(index 0) .
   	- The end point on the last way closest to the destination - this point must be a valid point on the last way in the vector.
    	- Distance and duration of the route(addition 15.12: from start point to end point as described above).



Assisting Mendatory Classes:
-
   Route:
   
	   std::vector<std::pair<EntityId, Direction>> all_ways
	   Coordinates start_point
	   Coordinates end_point
	   Meters total_length
	   Minutes est_duration

   Routes:
	   
	   Route shortest_distance;
	   Route shortest_time;
	   bool is_valid;

# Advanced-Topics-in-Programming-Simulator

