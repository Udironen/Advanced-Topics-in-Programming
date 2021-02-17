#include "NavigationValidator.h"


NavigationValidator::NavigationValidator(const AbstractGIS& gis_): gis(gis_) {}

bool NavigationValidator::validateRoute(
    const Coordinates& start, const Coordinates& end, const AbstractRoute& r) const{
   
    const auto& pairs_wayId_dir = r.getWays();
    if (pairs_wayId_dir.size() == 0) {	
         return false; }
    if (!validateEdgeRoute(start, r.getWayStartPoint())) { 	
        return false; }
    if (!validateEdgeRoute(end, r.getWayEndPoint())) { return false; }
    const AbstractWay& last_way = gis.getWay(pairs_wayId_dir[pairs_wayId_dir.size() - 1].first);
    Meters total_length = Meters(0);
    Meters length = Meters(0);
    Minutes total_time = Minutes(0);
    auto last_tuple = gis.getWayClosestPoint(r.getWayEndPoint());
    auto dist_from_to = last_way.getSegmentPartsOnWay(std::get<2>(last_tuple), std::get<0>(last_tuple));

    switch (pairs_wayId_dir[pairs_wayId_dir.size() - 1].second)
    {
    case Direction::B_to_A:
        if (!last_way.isBidirectional()) { 	
            return false; }
        total_length = dist_from_to.second;
        break;
    default:
        total_length = dist_from_to.first;
        break;
    }

    total_time = NavigationValidator::calcTime(total_length, last_way.getSpeedLimit());
    
    for (auto itr = pairs_wayId_dir.begin(); (itr+1) != pairs_wayId_dir.end(); itr++) {
        const AbstractWay& way = gis.getWay((itr)->first);
        const AbstractWay& next_way = gis.getWay((itr + 1)->first);
        switch (itr->second)
        {
        case Direction::B_to_A:
            if (!validateCaseBtoA(way, next_way, (itr + 1)->second)) { 	
            return false; }
            if (itr == pairs_wayId_dir.begin()) {
                auto first_tuple = gis.getWayClosestPoint(r.getWayStartPoint());
                dist_from_to = way.getSegmentPartsOnWay(std::get<2>(first_tuple), std::get<0>(first_tuple));
                length = dist_from_to.first;
                //std::cout << "tuple way id" << (std::string)std::get<1>(first_tuple) << std::endl;
                //std::cout << "tuple segment" << std::get<2>(first_tuple) << std::endl;
                //std::cout << " from " << (double)dist_from_to.first << " to " << (double)dist_from_to.second << std::endl;
                //std::cout << " validator first way " << (double)length << std::endl;
            }
            else {
                length = way.getLength();
            }
            break;
        default:
            if (!validateCaseAtoB(way, next_way, (itr + 1)->second)) { 	
                return false; }
            if (itr == pairs_wayId_dir.begin()) {
                auto first_tuple = gis.getWayClosestPoint(r.getWayStartPoint());
                dist_from_to = way.getSegmentPartsOnWay(std::get<2>(first_tuple), std::get<0>(first_tuple));
                length = dist_from_to.second;
                //std::cout << "tuple way id" << (std::string)std::get<1>(first_tuple) << std::endl;
                //std::cout << "tuple segment" << std::get<2>(first_tuple) << std::endl;
                //std::cout << " from " << (double)dist_from_to.first << " to " << (double)dist_from_to.second << std::endl;
                //std::cout << " validator first way " << (double)length << std::endl;


            }
            else {
                length = way.getLength();
            }
            break;
        }
        total_length = total_length + length;      

        total_time = total_time + NavigationValidator::calcTime(length , way.getSpeedLimit());
    }

    if (r.totalLength() != total_length) { 
    

        return false;
    }
    if (r.estimatedDuration() != total_time) {
        return false; 
    }

    return true;
}


bool NavigationValidator::isValidPoint(const AbstractWay& way, const Coordinates& point, const Coordinates& coordinate) const {
    Meters distance = CoordinatesMath::calculateDistance(point, coordinate);
    return !((way.isHighway()) && ((double)distance > 3));
}

bool NavigationValidator::validateCaseBtoA(const AbstractWay& way, const AbstractWay& next_way, const Direction next_way_dir) const {
    if (!way.isBidirectional()) { return false; }
    switch (next_way_dir)
    {
    case Direction::B_to_A:
        if (((std::string)way.getJunctions().first).compare((std::string)next_way.getJunctions().second) != 0) { return false; }
        break;
    default:
        if (((std::string)way.getJunctions().first).compare((std::string)next_way.getJunctions().first) != 0) { return false; }
        break;
    }
    return true;
}

bool NavigationValidator::validateCaseAtoB(const AbstractWay& way, const AbstractWay& next_way, const Direction next_way_dir) const {
    switch (next_way_dir)
    {
    case Direction::B_to_A:
        if (((std::string)way.getJunctions().second).compare((std::string)next_way.getJunctions().second) != 0) { return false; }
        break;
    default:
        if (((std::string)way.getJunctions().second).compare((std::string)next_way.getJunctions().first) != 0) { return false; }
        break;
    }
    return true;
}

bool NavigationValidator::validateEdgeRoute(const Coordinates& edge, const Coordinates point_on_way) const {
    Restrictions res = Restrictions("highway");
    auto point = gis.getWayClosestPoint(edge);
    const AbstractWay* way = &(gis.getWay(std::get<1>(point)));
    if (!isValidPoint(*way, std::get<0>(point), edge)) {
        point = gis.getWayClosestPoint(edge, res);
        way = &(gis.getWay(std::get<1>(point)));
        if (!isValidPoint(*way, std::get<0>(point), edge)) {
            
            return false;
        }
    }
    
    if (epsilon < (double)CoordinatesMath::calculateDistance(point_on_way, std::get<0>(point))) {
         return false;
    }
    return true;
}



    