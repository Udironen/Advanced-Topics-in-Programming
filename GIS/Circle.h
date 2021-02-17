#pragma once

#include "Shape.h"
#include "Entity.h"

class Circle: public Shape{
	Meters radius;
	Coordinates coordinates;

public:
		// constructors
		Circle(Meters radius_, const Coordinates& coor_);
		
		// get the radius of the circle
		Meters getRadius() const;
		
		// get the center of the circle
		Coordinates getCoordinates() const;

		// print the circle
		void print() const override;
};

