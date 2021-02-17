#pragma once 

#include "Cell.h"
#include "Way.h"
#include "../Common/CoordinatesMath.h"
#include "../UserCommon/Hashers.h"
#include<iterator>
#include <map>

typedef std::map<Latitude, std::map<Longitude, Cell, LongiCompare>, LatiCompare>::const_iterator Lati_iter;
typedef std::map<Longitude, Cell, LongiCompare>::const_iterator Longi_iter;

using GridCell = Coordinates;


class BaseGrid {

	friend class GIS_313452542;
	static constexpr double precision = 1;
	static constexpr double meridian_180 = 180;
	static constexpr double meridian_minus_180 = -180;
	static constexpr double s_pole = -90;
	static constexpr double n_pole = 90;

	std::map<Latitude, std::map<Longitude, Cell, LongiCompare>, LatiCompare> grid;

protected:
	//return the length of the of cell's side in the grid in meters
	Meters precisionInMeters() const;

	//truncate coordinate by precision
	double truncateDecimalCoordinate(const double coordinate) const;

	//truncate coordinates by precision
	Coordinates truncateCoordinates(const Coordinates& coordinates) const;

	//insert the name and id to the grid cell grid_cell
	 void insertToCell(	const GridCell& grid_cell,
								const std::string& name,
								const EntityId& id);

	//insert {name,id} to all the grid cells that the segment, that start at from and ends at to, passes through
	  void insertStraightRoad(	const Coordinates&	from,
								const Coordinates&	to,
								const std::string&	name,
								const EntityId&		id);

	  //insert an empty cell in the coordinates of grid_cell
	  void insertNothing(const GridCell& grid_cell);

public:
	//constructor, insert south and north pole into the grid
	BaseGrid();

	virtual ~BaseGrid() {}

	//insert the way to the grid
	//also need to get the coordinates of from and to of the way
	virtual void insert(Way&			way,
						const Coordinates&	from,
						const Coordinates&	to);

	//return grid.end()
	Lati_iter endLati();

	//return grid.find(grid_cell)
	 Lati_iter	findLati(const GridCell& grid_cell) const;
	Longi_iter	findLongi(const Lati_iter& lati_iter, const GridCell& grid_cell) const;
	
	//return grid.begin()
	Lati_iter beginLati();

	//clears the grid from all data
	void clear();

	//print the grid data
	void printGrid() const;


	//searching in circle that defined by starting_cell and radius 
	//center: the coordinats in the grid to begin the search
	//radius: the radius of the circle we are searching in
	//id_vector:the vector of id-s of ways in the circle, will be updated with the relevant id-s
	//memory_set: a set of id-s of ways we have already found (might not be in id_vector),
	//will be updated with the id-s that we find for the first time 
	void search(const	Coordinates&					center,
				const	Meters&							radius,
				const	std::string&					name,
						std::vector<EntityId>&			id_vector,
						std::unordered_set<EntityId>&	memory_set) const;

	//searching in the top half of the circle that defined by starting_cell and radius 
	//center: the coordinats in the grid to begin the search
	//radius: the radius of the circle we are searching in
	//id_vector:the vector of id-s of ways in the circle, will be updated with the relevant id-s
	//memory_set: a set of id-s of ways we have already found (might not be in id_vector),
	//will be updated with the id-s that we find for the first time 
	void searchInTop(	const	 Coordinates&					center,
						const	 Meters&						radius,
						const	std::string&					name,
								std::vector<EntityId>&			id_vector,
								std::unordered_set<EntityId>&	memory_set) const;

	//searching in the bottom half of the circle that defined by starting_cell and radius 
	//center: the coordinats in the grid to begin the search
	//radius: the radius of the circle we are searching in
	//id_vector:the vector of id-s of ways in the circle, will be updated with the relevant id-s
	//memory_set: a set of id-s of ways we have already found (might not be in id_vector),
	//	will be updated with the id-s that we find for the first time 
	void searchInBottom(const	Coordinates&					center,
						const	Meters&							radius,
						const	std::string&					name,
								std::vector<EntityId>&			id_vector,
								std::unordered_set<EntityId>&	memory_set) const;

	
	//searching in the right half of the circle that defined by starting_cell and radius 	
	void searchInRight(			Longi_iter						iter_longi,
						const	Coordinates&					center,
						const	Meters&							radius,
						const	std::string&					name,
								std::vector<EntityId>&			id_vector,
								std::unordered_set<EntityId>&	memory_set,
						const	Lati_iter&						iter_lati	) const;

	//searching in the left half of the circle that defined by starting_cell and radius 
	void searchInLeft(			Longi_iter						iter_longi,
						const	Coordinates&					center,
						const	Meters&							radius,
						const	std::string&					name,
								std::vector<EntityId>&			id_vector,
								std::unordered_set<EntityId>&	memory_set,
						const	Lati_iter&						iter_lati	) const;

	//searching in the cells in this latitude	
	void searchInLati(	const	Coordinates&					starting_cell,
						const	Meters&							radius,
						const	std::string&					name,
								std::vector<EntityId>&			id_vector,
								std::unordered_set<EntityId>&	memory_set, 
						const	Lati_iter&						iter_lati) const;

	//pushing to id_vector all the id-s that in id_set which are not in memory_set	
	void searchInCell(	const	std::unordered_set<EntityId>&	id_set,
									std::vector<EntityId>&			id_vector, 
									std::unordered_set<EntityId>&	memory_set) const;

	//updating lati_iter to the next iterator in cyclic order
	void getNextLatiIter(Lati_iter& lati_iter, bool& is_out_of_sphere)const;

	//updating lati_iter to the previous iterator in cyclic order
	void getPrevLatiIter(Lati_iter& lati_iter, bool& is_out_of_sphere) const;

	//updating iter_longi to the next iterator in cyclic order
	void getNextLongiIter(			Longi_iter&	iter_longi,
							const	Longi_iter& begin_of_lati, 
							const	Longi_iter& end_of_lati) const;

	//updating iter_longi to the previous iterator in cyclic order
	void getPrevLongiIter(			Longi_iter& iter_longi,
							const	Longi_iter& begin_of_lati,
							const	Longi_iter& end_of_lati) const;


	

};
	