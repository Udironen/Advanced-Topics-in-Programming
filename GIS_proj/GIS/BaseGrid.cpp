#include "BaseGrid.h"

BaseGrid::BaseGrid() 
{
	insertNothing(Coordinates(Longitude(0), Latitude(CoordinatesMath::n_pole_lat)));
	insertNothing(Coordinates(Longitude(0), Latitude(CoordinatesMath::s_pole_lat)));
}

Meters BaseGrid::precisionInMeters() const
{
	Coordinates zero_zero(Longitude(0), Latitude(0));
	Coordinates preci_zero(Longitude(0), Latitude(precision));
	Meters pres_meters = CoordinatesMath::calculateDistance(zero_zero, preci_zero);
	return pres_meters;
}


double BaseGrid::truncateDecimalCoordinate(const double coordinate) const
{
	return std::floor(coordinate / precision) * precision;
}

Coordinates BaseGrid::truncateCoordinates(const Coordinates& coordinates) const
{
	return	{	Longitude{truncateDecimalCoordinate((double)coordinates.longitude())},
				Latitude{truncateDecimalCoordinate((double)coordinates.latitude())}
			};
}

void BaseGrid::insertToCell(	const GridCell&		grid_cell,
									const std::string&	name, 
									const EntityId&		id)
{
	auto iter_lati = grid.find(grid_cell.latitude());
	if (iter_lati != grid.end()) {
		auto iter_longi = iter_lati->second.find(grid_cell.longitude());
		if (iter_longi != iter_lati->second.end()) {
			iter_longi->second.insert(name, id);
		}
		else {
			
			auto cell = iter_lati->second.insert({ grid_cell.longitude(),Cell() }).first;			
			cell->second.insert(name, id);

			
		}
	}
	else {
		auto longi_map = grid.insert({ grid_cell.latitude(), std::map<Longitude,Cell,LongiCompare>() }).first;
		auto cell = longi_map->second.insert({ grid_cell.longitude(),Cell() }).first;
		cell->second.insert(name, id);

	}
}

void BaseGrid::insert(	Way& way,
							const Coordinates& from,
							const Coordinates& to) 
{
	way.setFromJunctionCoordinates(from);
	way.setToJunctionCoordinates(to);
	std::vector<Coordinates> curves_vec;
	curves_vec.reserve(way.getCurves().size() + 2);
	curves_vec.push_back(from);
	for (const auto& coor : way.getCurves()) {
		curves_vec.push_back(coor);
	}
	curves_vec.push_back(to);
	Coordinates start = Coordinates(Longitude(0), Latitude(0));
	Coordinates end = Coordinates(Longitude(0), Latitude(0));
	for (size_t i = 0; i < curves_vec.size() - 1; i++) {
		start = curves_vec[i];
		end = curves_vec[i + 1];
		way.setLength(Meters((double)way.getLength() +
			(double)(CoordinatesMath::calculateDistance(start, end))));
		way.addPartialLength(way.getLength());
		insertStraightRoad(start, end,way.getEntityName(), way.getEntityId());
	}
}

void BaseGrid::insertStraightRoad(	const Coordinates&	from,
										const Coordinates&	to,
										const std::string&	name,
										const EntityId&		id)
{
	Meters precision_in_meters = precisionInMeters();
	Meters distance_from_to = CoordinatesMath::calculateDistance(from, to);
	Meters distance = Meters(0);
	GridCell curr_cell = truncateCoordinates(from);
	double bearing = CoordinatesMath::initialBearing(from, to);
	while (distance <= distance_from_to) {
		insertToCell(curr_cell, name, id);
		distance = Meters((double)distance + (double)precision_in_meters);
		curr_cell = truncateCoordinates(CoordinatesMath::coordinatesByBearingAndDistance(from, bearing, distance));
	}
}
void BaseGrid::printGrid() const 
{
	for (const auto& pair_lati : grid) {
		for (const auto& pair_longi : pair_lati.second) {			
			pair_longi.second.printCell();
		}
	}
}

void BaseGrid::insertNothing(const GridCell& grid_cell)
{
	auto iter_lati = grid.insert({ grid_cell.latitude(),std::map<Longitude,Cell,LongiCompare>() }).first;
	iter_lati->second.insert({ grid_cell.longitude(),Cell() }).first;
	iter_lati->second.insert({ Longitude(meridian_180),Cell() });
	iter_lati->second.insert({ Longitude(meridian_minus_180),Cell() });
}

Lati_iter BaseGrid::endLati() {
	return grid.end();
}
Lati_iter BaseGrid::beginLati() {
	return grid.begin();
}
 Lati_iter BaseGrid::findLati(const GridCell& grid_cell) const {
	 return grid.find(grid_cell.latitude());
	
}

Longi_iter BaseGrid::findLongi(const Lati_iter& lati_iter,const GridCell& grid_cell) const {
	return lati_iter->second.find(grid_cell.longitude());	
}

void BaseGrid::clear() {
	grid.clear();
}

void BaseGrid::getPrevLongiIter(		Longi_iter& iter_longi,
									const	Longi_iter& begin_of_lati,
									const	Longi_iter& end_of_lati) const
{
	if (iter_longi == begin_of_lati) {
		iter_longi = std::prev(end_of_lati);
	}
	else {
		iter_longi--;
	}
}

void BaseGrid::getNextLongiIter(		Longi_iter&	iter_longi,
									const	Longi_iter& begin_of_lati, 
									const	Longi_iter& end_of_lati) const
{
	iter_longi++;
	if (iter_longi == end_of_lati) { iter_longi = begin_of_lati; }
}

void BaseGrid::getPrevLatiIter(	Lati_iter& lati_iter,
									bool& is_out_of_sphere) const
{
	bool tmp = false;
	if (!is_out_of_sphere) {
		lati_iter--;
		if ((double)lati_iter->first == s_pole) {
			++lati_iter;
			tmp = true;
		}
	}
	else {
		++lati_iter;
	}
	is_out_of_sphere = tmp;
}

void BaseGrid::getNextLatiIter(	Lati_iter& lati_iter,
									bool& is_out_of_sphere)const
{
	bool tmp = false;
	if (!is_out_of_sphere) {
		lati_iter++;
		if ((double)lati_iter->first == n_pole) {
			--lati_iter;
			tmp = true;
		}
	}
	else {
		--lati_iter;
	}
	is_out_of_sphere = tmp;
}

void BaseGrid::searchInCell(const	std::unordered_set<EntityId>&	id_set,
										std::vector<EntityId>&			id_vector, 
										std::unordered_set<EntityId>&	memory_set) const
{
	for (const auto& id : id_set) {	

		if (memory_set.find(id) == memory_set.end()) {
			memory_set.insert(id);
			id_vector.push_back(id);
		}
	}
}

void BaseGrid::searchInRight(			Longi_iter						iter_longi,
									const	Coordinates&					center,
									const	Meters&							radius,
									const	std::string&					name,
											std::vector<EntityId>&			id_vector,
											std::unordered_set<EntityId>&	memory_set,
									const	Lati_iter&						iter_lati) const
{
	

	Meters sq2radius = std::sqrt(2) * radius;
	Meters longi_dist(0);
	Meters tmp_dist(0);

	auto begin_of_lati = iter_lati->second.begin();
	auto end_of_lati = iter_lati->second.end();
	while (longi_dist <= sq2radius) {

		auto iter_set = iter_longi->second.find(name);
		

		if (iter_set != iter_longi->second.nameIdMap.end()) {
			

			searchInCell(iter_set->second, id_vector, memory_set);
		}
		getNextLongiIter(iter_longi, begin_of_lati, end_of_lati);

		tmp_dist = CoordinatesMath::calculateDistance(center, Coordinates(iter_longi->first, iter_lati->first));
		if (tmp_dist <= longi_dist) {
			break;
		}
		longi_dist = tmp_dist;
	}

}

void BaseGrid::searchInLeft(			Longi_iter						iter_longi,
									const	Coordinates&					center,
									const	Meters&							radius,
									const	std::string&					name,
											std::vector<EntityId>&			id_vector,
											std::unordered_set<EntityId>&	memory_set,
									const	Lati_iter&						iter_lati) const
{
	Meters longi_dist(0);
	Meters sq2radius = std::sqrt(2) * radius;
	Meters tmp_dist(0);

	auto begin_of_lati = iter_lati->second.begin();
	auto end_of_lati = iter_lati->second.end();
	while (longi_dist <= sq2radius) {
		const auto& iter_set = iter_longi->second.nameIdMap.find(name);
		if (iter_set != iter_longi->second.nameIdMap.end()) {
			searchInCell(iter_set->second, id_vector, memory_set);
		}
		getPrevLongiIter(iter_longi, begin_of_lati, end_of_lati);

		tmp_dist = CoordinatesMath::calculateDistance(center, Coordinates(iter_longi->first, iter_lati->first));
		if (tmp_dist <= longi_dist) {
			break;
		}
		longi_dist = tmp_dist;
	}
}


void BaseGrid::searchInLati(const	Coordinates&					center,
								const	Meters&							radius,
								const	std::string&					name,
										std::vector<EntityId>&			id_vector,
										std::unordered_set<EntityId>&	memory_set, 
								const	Lati_iter&						iter_lati) const
{	
	bool longi_not_exist;
	auto grid_center = truncateCoordinates(center);
	auto iter_longi_start = iter_lati->second.find(grid_center.longitude());

	longi_not_exist = (iter_longi_start == iter_lati->second.end());
	
	if (longi_not_exist) {

		auto iter_longi_right = iter_lati->second.upper_bound(grid_center.longitude());
		auto longi_exist = (iter_longi_right != iter_lati->second.end());
		

		if (longi_exist) {
			searchInRight(iter_longi_right,
				center,
				radius,
				name,
				id_vector,
				memory_set,
				iter_lati);
		}	

		auto iter_longi_left = iter_lati->second.lower_bound(grid_center.longitude());
		longi_exist = (iter_longi_left != iter_lati->second.end());

		if (longi_exist) {
			getPrevLongiIter(iter_longi_left, iter_lati->second.begin(), iter_lati->second.end());
			searchInLeft(iter_longi_left,
				center,
				radius,
				name,
				id_vector,
				memory_set,
				iter_lati);
		}

	}
	else {
		searchInRight(iter_longi_start,
			center,
			radius,
			name,
			id_vector,
			memory_set,
			iter_lati);

		searchInLeft(iter_longi_start,
			center,
			radius,
			name,
			id_vector,
			memory_set,
			iter_lati);
	}	
}	
	

void BaseGrid::searchInTop(	const	Coordinates&					center,
								const	Meters&							radius,
								const	std::string&					name,
										std::vector<EntityId>&			id_vector,
										std::unordered_set<EntityId>&	memory_set) const
{
	GridCell start_cell = truncateCoordinates(center);
	auto iter_lati = grid.find(start_cell.latitude());
	bool is_up_outofsphere = false;
	Meters sq2radius = Meters(std::sqrt(2) * radius);
	Meters lati_dist = Meters(0);
	
	if (iter_lati == grid.end()) {
		iter_lati = grid.upper_bound(start_cell.latitude());
		if ((double)iter_lati->first == n_pole) {
			return;
		}
	}

	

	while (lati_dist <= sq2radius) {	
		
		searchInLati(	center,
						radius,
						name,
						id_vector,
						memory_set,
						iter_lati);
		

		getNextLatiIter(iter_lati, is_up_outofsphere);

		if (!is_up_outofsphere) {
			start_cell = truncateCoordinates(Coordinates(center.longitude(), iter_lati->first));
		}
		else {
			if ((double)iter_lati->first >= 0) {
				start_cell = truncateCoordinates(Coordinates(Longitude((double)(center.longitude()) - meridian_180), iter_lati->first));
			}
			else {
				start_cell = truncateCoordinates(Coordinates(Longitude((double)(center.longitude()) + meridian_180), iter_lati->first));
			}
		}
		auto tmp_dist = CoordinatesMath::calculateDistance(center, start_cell);
		if (tmp_dist <= lati_dist) {
			break;
		}
		lati_dist = tmp_dist;
	}
}

void BaseGrid::searchInBottom(	const	Coordinates&					center,
									const	Meters&							radius,
									const	std::string&					name,
											std::vector<EntityId>&			id_vector,
											std::unordered_set<EntityId>&	memory_set) const
{
	GridCell start_cell = truncateCoordinates(center);
	auto iter_lati = findLati(start_cell);
	bool is_up_outofsphere = false;
	Meters sq2radius = Meters(std::sqrt(2) * radius);
	Meters lati_dist = Meters(0);
	
	if (iter_lati == grid.end()) {
		iter_lati = grid.lower_bound(start_cell.latitude());
		getPrevLatiIter(iter_lati, is_up_outofsphere);
		if ((double)iter_lati->first == s_pole) {
			return;
		}
	}
	

	while (lati_dist <= sq2radius) {

		searchInLati(	center,
						radius,
						name,
						id_vector,
						memory_set,
						iter_lati);
		

		getPrevLatiIter(iter_lati, is_up_outofsphere);

		if (!is_up_outofsphere) {
			start_cell = truncateCoordinates(Coordinates(center.longitude(), iter_lati->first));
		}
		else {
			if ((double)iter_lati->first >= 0) {
				start_cell = truncateCoordinates(Coordinates(Longitude((double)(center.longitude()) - meridian_180), iter_lati->first));
			}
			else {
				start_cell = truncateCoordinates(Coordinates(Longitude((double)(center.longitude()) + meridian_180), iter_lati->first));
			}
		}
		auto tmp_dist = CoordinatesMath::calculateDistance(center, start_cell);
		if (tmp_dist <= lati_dist) {
			break;
		}
		lati_dist = tmp_dist;
	}
}
