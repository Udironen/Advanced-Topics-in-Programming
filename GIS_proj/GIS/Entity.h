#pragma once

#include "../Common/GISNamedTypes.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>

class Entity{
public:
	enum EntityType { Junction, POI, Way };

protected:
	EntityId id;
	std::string name;
	std::string description;
	std::set<std::string> category_tags;
	EntityType type;
	friend class GIS_313452542;

	void setID(const EntityId& id_) {
		id = id_;
	}

	

	
public: 

	// constructor
	Entity(	const	EntityId&				id_,
			const	std::string&			name_,
			const	std::string&			description_,
			const	std::set<std::string>&	tags_,
					EntityType type_ ) :

		id(id_), name(name_), description(description_), category_tags(tags_), type(type_) {}
	
	// get the id of entity
	EntityId getEntityId() const {
		return id;
	}

	void setName(const std::string& name_) {
		name = name_;
	}

	EntityType getType() const {
		return type;
	}
	
	// get the name of entity
	std::string getEntityName() const {
		return name;
	}
	
	// get the description of entity
	std::string getEntityDescription() const{
		return description;
	}

	// get the category tag of entity
	const std::set<std::string>& getEntityCategoryTag() const{
		return category_tags;
	}

	//print coordinates
	static void printCoordinates(const Coordinates coords) {
		std::cout << "[" << (double)coords.longitude() << ", ";
		std::cout << (double)coords.latitude() << "]\n" ;
	}

	// print the entity
	virtual void print() const{};
};

