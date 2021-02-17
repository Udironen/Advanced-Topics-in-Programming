#include "Circle.h"

Circle::Circle(	Meters radius_,const Coordinates& coor_) :
				radius(radius_), coordinates(coor_) {}

Meters Circle::getRadius() const {
	return radius;
}
Coordinates Circle::getCoordinates() const {
	return coordinates;
}

void Circle::print() const {
	std::cout <<(double)radius << ", ";
	Entity::printCoordinates(coordinates);
}