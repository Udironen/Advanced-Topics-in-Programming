#pragma once

#include "../Common/GISNamedTypes.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include<vector>


class Cell {
	friend class BaseGrid;
	friend class GIS_313452542;
	std::unordered_map<std::string, std::unordered_set<EntityId>> nameIdMap;

public:

	// insert entity name and id to the container nameIdMap
	void insert(const std::string& name, const EntityId id) {
		auto it = nameIdMap.find(name);
		if (it != nameIdMap.end()) {
			it->second.insert(id);
		}
		else {
			nameIdMap.insert({ name, std::unordered_set<EntityId>({id}) });
		}
	}

	std::unordered_map<std::string, std::unordered_set<EntityId>>::const_iterator find(const std::string& name) const {
		auto itr = nameIdMap.find(name);
		return itr;
	}


	// print cell
	void printCell() const {
		for (auto const& pair : nameIdMap) {
			std::cout << "\tname: " << pair.first << "\n\tid vector:[";
			for (const auto& id : pair.second) {
				std::cout << (std::string)id << ", ";
			}
			std::cout << "]";
		}
	}
};