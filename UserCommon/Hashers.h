#pragma once

#include "../Common/GISNamedTypes.h"
#include "../Navigation/Node.h"
#include "../Common/AbstractRoutes.h"
#include "../Simulator/GISNode.h"
#include "../Simulator/Group.h"
#include <unordered_map>

struct IDhash {
	std::size_t operator()(const EntityId& id) const noexcept {
		return std::hash<std::string>{}((std::string)id);
	}
};

struct GISNodehash {
	std::size_t operator()(const GISNode& gis_node) const noexcept {
		return std::hash<std::string>{}(gis_node.getGISName());
	}
};


struct Minuteshash {
	std::size_t operator()(const Minutes minutes) const noexcept {
		return std::hash<double>{}((double)minutes);
	}
};

struct Metershash {
	std::size_t operator()(const Meters meters) const noexcept {
		return std::hash<double>{}((double)meters);
	}
};


typedef std::unordered_map<EntityId, Node, IDhash>::const_iterator iter_node;

struct CoorCompare {
	bool operator() (const Coordinates& lhs, const Coordinates& rhs) const {
		
		if (lhs.longitude() < rhs.longitude()) {
			return true;
		}
		else if (rhs.longitude() < lhs.longitude()) {
			return false;
		}
		return(lhs.latitude() < rhs.latitude());

	}
};

struct LongiCompare {
	bool operator() (const Longitude& lhs, const Longitude& rhs) const {

		return (lhs < rhs);
	}
};
struct LatiCompare {
	bool operator() (const Latitude& lhs, const Latitude& rhs) const {

		return (lhs < rhs);
	}
};

struct PairCompare{
	bool operator()(const std::pair<double,iter_node> lhs, const std::pair<double,iter_node> rhs){
		return (lhs.first > rhs.first) ;
	}
};

typedef std::tuple<std::string&, std::unique_ptr<AbstractRoutes>, std::size_t> tuple;

struct TupleCompare {
	bool operator()(const tuple& lhs, const tuple& rhs) {
		return (std::get<2>(lhs) > std::get<2>(rhs));
	}
};

struct PairCoordinatesHash {
	std::size_t operator()(const std::pair<Coordinates,Coordinates>& pair) const noexcept {
		auto h1 = std::hash<double>{}((double)pair.first.latitude());
		auto h2 = std::hash<double>{}((double)pair.first.longitude());
		auto h3 = std::hash<double>{}((double)pair.second.longitude());
		auto h4 = std::hash<double>{}((double)pair.second.latitude());
		return h1 ^ h2 ^ h3 ^ h4;
	}
};
template<class key_Type, template<class T> class Groupe_Type>
struct GroupCompare {
	bool operator() (const Groupe_Type<key_Type>& lhs, const Groupe_Type<key_Type>& rhs) const {
		
		return (lhs > rhs);
	}
};


