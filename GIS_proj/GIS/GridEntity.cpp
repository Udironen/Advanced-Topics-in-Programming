#include "GridEntity.h"



void GridEntity::insert(const Junction& junc)
{
	GridCell grid_cell = truncateCoordinates(junc.getCoordinates());
	insertToCell(grid_cell, junc.getEntityName(),junc.getEntityId());
}

void GridEntity::insert(const POI& poi)
{
	std::string name = poi.getEntityName();
	EntityId id = poi.getEntityId();
	const Circle& circle = poi.getShape();
	insertPOITopSection(name, id, circle);	
	insertPOIBottomSection(name, id, circle);
}

void GridEntity::insertPOITopSection(	const std::string&	name,
										const EntityId&		id,
										const Circle&		circle) 
{
	Meters radius = circle.getRadius();
	Meters precision_in_meters =precisionInMeters();
	Coordinates center = circle.getCoordinates();	
	GridCell grid_center = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(center, UP, precision_in_meters));
	Meters curr_radius_longi = precision_in_meters;
	Meters curr_radius_lati = precision_in_meters;
	GridCell curr_grid = grid_center;

	while ((double)curr_radius_lati <= (double)radius) {
		curr_grid = grid_center;
		insertToCell(grid_center, name, id);
		while ((double)curr_radius_longi <= (double)radius) {			
			curr_grid = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(grid_center, LEFT, curr_radius_longi));
			insertToCell(curr_grid, name, id);
			curr_grid = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(grid_center, RIGHT, curr_radius_longi));
			insertToCell(curr_grid, name, id);
			curr_radius_longi = Meters((double)curr_radius_longi + (double)precision_in_meters);
		}
		curr_radius_lati = Meters((double)curr_radius_lati + (double)precision_in_meters);
		grid_center = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(center, UP, curr_radius_lati));
		curr_radius_longi = Meters(precision_in_meters);
	}	
}

void GridEntity::insertPOIBottomSection(const std::string&	name,
										const EntityId&		id,
										const Circle&		circle)
{
	Meters radius = circle.getRadius();
	Meters precision_in_meters = precisionInMeters();
	Coordinates center = circle.getCoordinates();
	GridCell grid_center = truncateCoordinates(center);
	Meters curr_radius_longi = precision_in_meters;
	Meters curr_radius_lati = Meters(0);
	GridCell curr_grid = grid_center;	
	
	while ((double)curr_radius_lati <= (double)radius) {
		curr_grid = grid_center;
		insertToCell(grid_center, name, id);
		while ((double)curr_radius_longi <= (double)radius) {
			curr_grid = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(grid_center, LEFT, curr_radius_longi));
			insertToCell(curr_grid, name, id);
			curr_grid = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(grid_center, RIGHT, curr_radius_longi));
			insertToCell(curr_grid, name, id);
			curr_radius_longi = Meters((double)curr_radius_longi + (double)precision_in_meters);
		}
		curr_radius_lati = Meters((double)curr_radius_lati + (double)precision_in_meters);
		grid_center = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(center, DOWN, curr_radius_lati));
		curr_radius_longi = Meters(precision_in_meters);
	}	
}

void GridEntity::insert(Way& way,
						const Coordinates& from,
						const Coordinates& to)
{
	BaseGrid::insert(way, from, to);
}

