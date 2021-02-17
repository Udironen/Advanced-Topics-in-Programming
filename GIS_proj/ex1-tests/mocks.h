#pragma once 

#include "../Common/GISNamedTypes.h"
#include <string> 
#include <vector>

/**
 * @brief Returns input file entites in order defined
 * 
 * @return std::vector<EntityId> 
 */
const std::vector<EntityId>& getInputFileEntities(const std::string& filename) {
	if (filename == "nyc.json") {
		static std::vector<EntityId> nyc_entities({EntityId("P1001"), EntityId("P1002"), EntityId("J1001"), EntityId("J1002"), EntityId("J1010"), EntityId("P1010"), EntityId("W2001"), EntityId("W2002")});	
		return nyc_entities;
	}
	if (filename == "ex1_map1.json") {
		static std::vector<EntityId> ex1_map1_entites({EntityId("Pnt1"), EntityId("Ju01"), EntityId("Ju02"), EntityId("Ju04"), EntityId("Ju03"), EntityId("West1"), EntityId("East1")});
		return ex1_map1_entites;
	}
	if (filename == "ex1_map2.json") {
		static std::vector<EntityId> ex1_map2_entites({EntityId("P1"), EntityId("J1"), EntityId("J2"), EntityId("J3"), EntityId("J4"), EntityId("W1"), EntityId("W2")});	
		return ex1_map2_entites; 
	} 
	if (filename == "ex1_two_ways.json" || filename == "ex1_two_ways_180th_meridian.json") {
		static std::vector<EntityId> ex1_two_ways({EntityId("TL"), EntityId("BL"), EntityId("LeftWay"), EntityId("TR"), EntityId("BR"), EntityId("RightWay")});	
		return ex1_two_ways; 
	} 
	if (filename == "ex2_map1.json") {
		static std::vector<EntityId> ex2_map1_entites({EntityId("P1"), EntityId("J1"), EntityId("J2"), EntityId("J3"), EntityId("J4"), EntityId("J5"), EntityId("P2"), EntityId("W1"), EntityId("W2"), EntityId("W3"), EntityId("W4"), EntityId("W5")});	
		return ex2_map1_entites; 
	} 
	//Empty file
	static std::vector<EntityId> empty_vector{};
	return empty_vector;
}

/**
 * @brief Get Entities object of both files in undefined order - eliminates duplicates
 * 
 * @param filename1 
 * @param filename1 
 * @return std::vector<EntityId> 
 */
std::vector<EntityId> getCombinedFilesEntities(const std::string& filename1, const std::string& filename2) {
	auto map1_entities = getInputFileEntities(filename1);
	auto map2_entities = getInputFileEntities(filename2);
	std::vector<EntityId> merged_entities; 
	merged_entities.reserve(map1_entities.size() + map2_entities.size()); // preallocate memory
	merged_entities.insert(merged_entities.end(), map1_entities.begin(), map1_entities.end());
	merged_entities.insert(merged_entities.end(), map2_entities.begin(), map2_entities.end());
	// Clear duplicates
	std::sort(merged_entities.begin(), merged_entities.end());
	merged_entities.erase(std::unique(merged_entities.begin(), merged_entities.end()), merged_entities.end());
	return merged_entities;
}

namespace gis_testing {
	struct Way {
		Coordinates from; 
		Coordinates to; 
	};
}

/**
 * @brief ex1_map2.json
 * 
 */
gis_testing::Way ex1_map2_W2{{Longitude{-30}, Latitude{-10}}, {Longitude{30}, Latitude{-10}}};

/**
 * @brief ex1_two_ways
 * 
 */
Coordinates ex1_two_ways_TL{Longitude{44.361}, Latitude{33.312}};
Coordinates ex1_two_ways_BL{Longitude{44.361}, Latitude{27.312}};
Coordinates ex1_two_ways_TR{Longitude{50.361}, Latitude{33.312}};
Coordinates ex1_two_ways_BR{Longitude{50.361}, Latitude{27.312}};

gis_testing::Way ex1_two_ways_LeftWay{ex1_two_ways_BL, ex1_two_ways_TL};
gis_testing::Way ex1_two_ways_RightWay{ex1_two_ways_BR, ex1_two_ways_TR};

/**
 * @brief ex1_two_ways_180th_meridan
 * 
 */
Coordinates ex1_two_ways_180th_meridan_TL{Longitude{44.361}, Latitude{33.312}};
Coordinates ex1_two_ways_180th_meridan_BL{Longitude{44.361}, Latitude{27.312}};
Coordinates ex1_two_ways_180th_meridan_TR{Longitude{50.361}, Latitude{33.312}};
Coordinates ex1_two_ways_180th_meridan_BR{Longitude{50.361}, Latitude{27.312}};

gis_testing::Way ex1_two_ways_180th_meridan_LeftWay{ex1_two_ways_180th_meridan_BL, ex1_two_ways_180th_meridan_TL};
gis_testing::Way ex1_two_ways_180th_meridan_RightWay{ex1_two_ways_180th_meridan_BR, ex1_two_ways_180th_meridan_TR};

