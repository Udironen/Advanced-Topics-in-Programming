#pragma once

#include "BaseGrid.h"
#include "Junction.h"
#include "POI.h"

#define LEFT  270
#define RIGHT  90
#define DOWN  180
#define UP		0


class GridEntity : public BaseGrid {
	
	
	//inserting  {name,id} to the cells that in poligon that tangent the top half of the circle from outside  
	void insertPOITopSection(	const std::string&	name,
								const EntityId&		id,
								const Circle&		circle);
	
	//inserting  {name,id} to the cells that in poligon that tangent the bottom half of the circle from outside  
	void insertPOIBottomSection(const std::string&	name,
								const EntityId&		id,
								const Circle&		circle);
	
public:
	~GridEntity() = default;

	using BaseGrid::insert;

	//insert the name and id of junc to the grid
	void insert(const Junction& junc);

	//insert the name and id of poi to the grid
	void insert(const POI& poi);

	//insert the name and id of way to the grid,
	//also need to get the coordinates of from and to of the way
	void insert(Way& way, const Coordinates& from, const Coordinates& to);

	

};