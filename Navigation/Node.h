#pragma once
#include<memory>
#include "../Common/GISNamedTypes.h"

class Node {
	
		
	EntityId id;
	std::shared_ptr<Node> parent;
	EntityId way_from_parent_id;
	Direction direction;
	Meters length_so_far;
	Minutes time_so_far;
	double heuristic;
	Coordinates coords;
	std::size_t num_of_ways;

public:
	static double constexpr one_km = 1000;
	static double constexpr one_hour = 60;
	static int constexpr max_speed_limit = 130;
	static double constexpr defult_longi = 0;
	static double constexpr defult_lati = 90;

		Node(	const	EntityId& id_,
				const	Node& parent_,
				const	EntityId& way_from_parent_id_,
				const	Direction& direction_,
						Meters length_so_far_,
						Minutes time_so_far_, 
						double heuristic_,
				const	Coordinates& coords_)
			:
			id					(id_),
			way_from_parent_id	(way_from_parent_id_),
			direction			(direction_),
			length_so_far		(length_so_far_),
			time_so_far			(time_so_far_),
			heuristic			(heuristic_),
			coords				(coords_),
			num_of_ways			(1e9)
			
		{
			parent = std::make_shared<Node>(parent_);
			
		}
		Node():id(EntityId("")), parent(nullptr), way_from_parent_id(EntityId("")), 
				direction(Direction::A_to_B), length_so_far(Meters(0)), time_so_far(Minutes(0)),
				 heuristic(0), coords(Coordinates(Longitude(defult_longi),Latitude(defult_lati))) {}

		
		size_t getNumOfWay() const {
			return num_of_ways;
		}
		void increaseNumOfWays(){
			num_of_ways++;
		}

		void setNumOfWay(size_t num){
			num_of_ways = num;
		}

		EntityId getId() const {
			return id;
		}
		void setId(const EntityId& id_) {
			id = id_ ;
		}
		std::shared_ptr<Node> getParent() const {
			return parent;
		}
		void setParent(const Node& parent_) {
			parent = std::make_shared<Node>(parent_);
		}

		EntityId getWayFromParentId() const {
			return way_from_parent_id;
		}

		void setWayFromParentId(const EntityId& way_from_parent_id_) {
			way_from_parent_id = way_from_parent_id_;
		}

		Direction getDirection() const {
			return direction;
		}

		void setDirection( Direction direction_) {
			direction = direction_;
		}

		Meters getCostLength() const {
			return length_so_far;
		}

		void setCostLength(Meters length_so_far_) {
			length_so_far = length_so_far_;
		}

		Minutes getCostTime() const {
			return time_so_far;
		}

		void setCostTime(Minutes time_so_far_) {
			time_so_far = time_so_far_;
		}

		double getHeuristic(bool is_time = false) const {
			if (is_time) {
				return (double)calcTime(Meters(heuristic), max_speed_limit);
			}
			else {
				return heuristic;
			}
		}
		
		double getPriority(bool is_time=false) const {
			if (is_time) {
				return getHeuristic(is_time) + (double)(time_so_far) ;
			}
			else {
				return heuristic + (double)length_so_far ;
			}
			
		}

		Coordinates getCoordinates() const {
			return coords;
		}

		

		static Minutes calcTime(Meters dist, int speed) {
			double time = (double)(dist) / (double)(one_km);
			time /= (double)speed;
			time *= (double)one_hour;
			return Minutes(time);
		}
	
};