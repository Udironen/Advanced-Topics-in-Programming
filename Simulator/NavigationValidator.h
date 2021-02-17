#pragma once
#include "../Common/AbstractRoute.h"
#include "../Common/AbstractGIS.h"
#include "../Common/CoordinatesMath.h"
#include "../Common/NavigationGIS.h"

#include <iostream>
class NavigationValidator {
    NavigationGIS gis;
    static constexpr double epsilon = 0.1;

    bool validateEdgeRoute(const Coordinates& edge, const Coordinates point_on_way) const;

    bool validateCaseBtoA(const AbstractWay& way, const AbstractWay& next_way, const Direction next_way_dir) const;

    bool validateCaseAtoB(const AbstractWay& way, const AbstractWay& next_way, const Direction next_way_dir) const;

    bool isValidPoint(const AbstractWay& way, const Coordinates& point, const Coordinates& coordinate) const;

public:
    static double constexpr one_km = 1000;
	static double constexpr one_hour = 60;
	static int constexpr max_speed_limit = 130;
	static double constexpr defult_longi = 0;
	static double constexpr defult_lati = 90;
    //constructor
    NavigationValidator(const AbstractGIS& gis_); // note that you get a const ref
    
    // check if the route is valid
    bool validateRoute( const Coordinates& start, const Coordinates& end, const AbstractRoute& r) const;
   
    static Minutes calcTime( Meters dist, int speed) {
			double time = (double)(dist) / (double)(one_km);
			time /= (double)speed;
			time *= (double)one_hour;
			return Minutes(time);
	}
};
