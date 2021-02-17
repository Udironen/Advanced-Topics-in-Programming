#include <cstdio>
#include "gtest/gtest.h"

#include "mocks.h"

// Ex1
#include "../GIS/GISProvider.h"
#include "../Common/CoordinatesMath.h"

// Pre tests

TEST(GISPre, loadMapFile){
	auto gis = GISProvider::getGIS();
	std::string input_file{"nyc.json"};
	std::vector<EntityId> loaded_entities = gis.loadMapFile(input_file); 
	EXPECT_EQ(loaded_entities, getInputFileEntities(input_file));
}

TEST(GISPre, getEntities) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"nyc.json"};
	gis.loadMapFile(input_file);
	std::vector<EntityId> entities = gis.getEntities("Washington Square Park");
	std::sort(entities.begin(), entities.end());
	std::vector<EntityId> expected_entities({EntityId("P1001")}); 
    	EXPECT_EQ(entities, expected_entities);
}

TEST(GISPRe, clearReturnValue) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"nyc.json"};
    	gis.loadMapFile(input_file);
    	std::size_t number_of_cleared_entities = gis.clear();
    	EXPECT_EQ(number_of_cleared_entities, getInputFileEntities(input_file).size());
}

TEST(GISPre, saveMapFileReturnValue) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"nyc.json"};
	std::string output_file{"out1."+input_file};
    	gis.loadMapFile(input_file);
	std::size_t number_of_saved_entities = gis.saveMapFile(output_file);
	EXPECT_EQ(number_of_saved_entities, getInputFileEntities(input_file).size());
	std::remove(output_file.c_str());
}

// Ex1 Tests

TEST(GISEx1, saveMapFileLoadSavedMap) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"nyc.json"};
    	gis.loadMapFile(input_file);
	std::string output_file{"out2."+input_file};
	std::size_t number_of_saved_entities = gis.saveMapFile(output_file);
	EXPECT_EQ(number_of_saved_entities, getInputFileEntities(input_file).size());
	gis.clear();
	auto loaded_entities = gis.loadMapFile(input_file);
	std::sort(loaded_entities.begin(), loaded_entities.end());
	std::vector<EntityId> expected_entites = getInputFileEntities(input_file);
	std::sort(expected_entites.begin(), expected_entites.end());
	EXPECT_EQ(loaded_entities, expected_entites);
	std::remove(output_file.c_str());
}

TEST(GISEx1, clearLoad) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"nyc.json"};
    	gis.loadMapFile(input_file);
    	gis.clear();
	std::vector<EntityId> loaded_entities = gis.loadMapFile("nyc.json"); 
	EXPECT_EQ(loaded_entities, getInputFileEntities(input_file));
}

TEST(GISEx1, loadMapFileMap1) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"ex1_map1.json"};
	std::vector<EntityId> loaded_entities = gis.loadMapFile(input_file);
	EXPECT_EQ(loaded_entities, getInputFileEntities(input_file));
}

TEST(GISEx1, loadMapFileMap2) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"ex1_map2.json"};
	std::vector<EntityId> loaded_entities = gis.loadMapFile(input_file);
	EXPECT_EQ(loaded_entities, getInputFileEntities(input_file));
}

TEST(GISEx1, loadMapFileMap1NoIds) {
    	auto gis = GISProvider::getGIS();
	std::vector<EntityId> loaded_entities = gis.loadMapFile("ex1_map1.no_ids.json");
	std::size_t number_of_loaded_entities = loaded_entities.size();
	std::size_t expected_number_of_loaded_entities = 5; 
	EXPECT_EQ(number_of_loaded_entities, expected_number_of_loaded_entities);
}

TEST(GISEx1, loadMapFileMap1MixedIds) {
    	auto gis = GISProvider::getGIS();
	std::vector<EntityId> loaded_entities = gis.loadMapFile("ex1_map1.mixed_ids_file.json");
	std::size_t number_of_loaded_entities = loaded_entities.size();
	std::set<std::size_t> expected_number_of_loaded_entities({1, 6, 7}); 
	EXPECT_TRUE(expected_number_of_loaded_entities.count(number_of_loaded_entities));
}

TEST(GISEx1, twoLoadsSameFile) {
    	auto gis = GISProvider::getGIS();
	gis.loadMapFile("ex1_map1.json");
	gis.loadMapFile("ex1_map1.json");
	std::vector<EntityId> entities = gis.getEntities("LWS");
	std::vector<EntityId> expected_entities({EntityId("Ju02")});
	EXPECT_EQ(entities, expected_entities);
}

TEST(GISEx1, twoLoadsMap1Map2) {
    	auto gis = GISProvider::getGIS();
	std::string input_file1{"ex1_map1.json"};
	std::vector<EntityId> loaded_entities1 = gis.loadMapFile(input_file1);
	EXPECT_EQ(loaded_entities1, getInputFileEntities(input_file1));
	std::string input_file2{"ex1_map2.json"};
	std::vector<EntityId> loaded_entities2 = gis.loadMapFile(input_file2);
	EXPECT_EQ(loaded_entities2, getInputFileEntities(input_file2));
}

TEST(GISEx1, twoLoadsMap1Map2AndGetEntities) {
    	auto gis = GISProvider::getGIS();
	std::string input_file1{"ex1_map1.json"};
	gis.loadMapFile(input_file1);
	std::string input_file2{"ex1_map2.json"};
	gis.loadMapFile(input_file2);
	std::vector<EntityId> entities = gis.getEntities("LWS");
	std::vector<EntityId> expected_entities({EntityId("J2"), EntityId("Ju02")});
	std::sort(entities.begin(), entities.end());
	EXPECT_EQ(entities, expected_entities);
}

constexpr double ex1_epsilon = 1;
template <typename LL> //TODO: Use LatLong in ex2 
bool testLatLongEqualityInEx1(LL tested, LL golden) {
	return std::fabs(static_cast<double>(tested) - static_cast<double>(golden)) < ex1_epsilon;
}

bool testCoordinatsEqualityInEx1(const Coordinates tested, const Coordinates& golden) {
	return testLatLongEqualityInEx1(tested.longitude(), golden.longitude()) &&
		testLatLongEqualityInEx1(tested.latitude(), golden.latitude());
}

TEST(GISEx1, getEntityClosestPointNotKnown) {
	// [1] If the Id is not known returns an empty optional
    	auto gis = GISProvider::getGIS();
    	gis.loadMapFile("ex1_map1.json");
	Coordinates point{Longitude{40.0}, Latitude{10.0}};
	Coordinates pnt_center{Longitude{20.0}, Latitude{70.0}};
	Meters pnt_radius{1001.96};
	std::optional<Coordinates> closest_point = gis.getEntityClosestPoint(EntityId{"non-exixtant"}, point);
	ASSERT_FALSE(closest_point);
}

TEST(GISEx1, getEntityClosestPoint1) {
    	auto gis = GISProvider::getGIS();
    	gis.loadMapFile("ex1_map1.json");
	Coordinates point{Longitude{40.0}, Latitude{10.0}};
	Coordinates pnt_center{Longitude{20.0}, Latitude{70.0}};
	Meters pnt_radius{1001.96};
	std::optional<Coordinates> closest_point = gis.getEntityClosestPoint(EntityId{"Pnt1"}, point);
	Coordinates expected_point{Longitude{20.0101094228863}, Latitude{69.9916792225197}};
	ASSERT_TRUE(closest_point);
	EXPECT_TRUE(testCoordinatsEqualityInEx1(*closest_point, expected_point));
}

TEST(GISEx1, getEntityClosestPoint2) {
    	auto gis = GISProvider::getGIS();
    	gis.loadMapFile("ex1_map2.json");
	Coordinates point{Longitude{-1}, Latitude{-1}};
	Coordinates pnt_center{Longitude{20.730811}, Latitude{10.997455}};
	Meters pnt_radius{100.96};
	std::optional<Coordinates> closest_point = gis.getEntityClosestPoint(EntityId{"P1"}, point);
	Coordinates expected_point{Longitude{20.7299920271604}, Latitude{10.9970330126177}};
	ASSERT_TRUE(closest_point);
	EXPECT_TRUE(testCoordinatsEqualityInEx1(*closest_point, expected_point));
}

TEST(GISEx1, getWayClosestPoint) {
    	auto gis = GISProvider::getGIS();
    	gis.loadMapFile("ex1_map1.json");
	Coordinates east_point{Longitude{15}, Latitude{-50}};
	auto closest = gis.getWayClosestPoint(east_point);
	EXPECT_EQ(std::get<AbstractGIS::ClosestPoint::WayId>(closest), EntityId{"East1"});
	Coordinates ju_04{Longitude{20}, Latitude{-20.0}};
	EXPECT_EQ(std::get<AbstractGIS::ClosestPoint::Coord>(closest), ju_04);
}

TEST(GISEx1, getWayClosestPointNorth) {
    	auto gis = GISProvider::getGIS();
    	gis.loadMapFile("ex1_map2.json");
	Coordinates northern_point{Longitude{-1}, Latitude{60}};
	auto closest_from_gis = gis.getWayClosestPoint(northern_point);
	EXPECT_EQ(std::get<AbstractGIS::ClosestPoint::WayId>(closest_from_gis), EntityId{"W1"});
	Coordinates closest_to_northern_way{Longitude{-3.55086}, Latitude{81.3014}};
	EXPECT_TRUE(testCoordinatsEqualityInEx1(std::get<AbstractGIS::ClosestPoint::Coord>(closest_from_gis), closest_to_northern_way));
}

TEST(GISEx1, getWayClosestPointSouth) {
    	auto gis = GISProvider::getGIS();
   	gis.loadMapFile("ex1_map2.json");
	Coordinates southern_point{Longitude{21}, Latitude{-1}};
	auto closest_from_gis = gis.getWayClosestPoint(southern_point);
	Coordinates closest_to_southern_way{Longitude{21.7155}, Latitude{-10.7113}};
	EXPECT_TRUE(testCoordinatsEqualityInEx1(std::get<AbstractGIS::ClosestPoint::Coord>(closest_from_gis), closest_to_southern_way));
}

TEST(GISEx1, getEntities) {
    	auto gis = GISProvider::getGIS();
	gis.loadMapFile("ex1_map2.json");
	std::vector<EntityId> entities = gis.getEntities("A_way");
	std::vector<EntityId> expected_entities({EntityId("W1"), EntityId("W2")});
	std::sort(entities.begin(), entities.end());
    	EXPECT_EQ(entities, expected_entities);
}

TEST(GISEx1, getEntitiesRadius) {
    	auto gis = GISProvider::getGIS();
	gis.loadMapFile("ex1_map2.json");
	Coordinates point{Longitude{-20}, Latitude{-9.94}};
	Meters circle_radius{100'000};
	std::vector<EntityId> entities = gis.getEntities("A_way", point, circle_radius);
	auto distance_from_way = CoordinatesMath::distanceFromSegment(point, ex1_map2_W2.from, ex1_map2_W2.to);
	EXPECT_TRUE(distance_from_way <= circle_radius); // Sanity check
	std::vector<EntityId> expected_entities({EntityId("W2")});
    	EXPECT_EQ(entities, expected_entities);
}

TEST(GISEx1, loadMap1SaveClearAndLoad) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"ex1_map1.json"};
	auto loaded_input = gis.loadMapFile(input_file);
	std::sort(loaded_input.begin(), loaded_input.end());
	std::string output_file{"out."+input_file};
	std::size_t saved = gis.saveMapFile(output_file);
	std::size_t number_of_cleared_entities = gis.clear();
	EXPECT_TRUE(number_of_cleared_entities == saved && saved == getInputFileEntities(input_file).size());
	auto loaded_output = gis.loadMapFile(output_file);
	std::sort(loaded_output.begin(), loaded_output.end());
	EXPECT_EQ(loaded_input, loaded_output);
	std::remove(output_file.c_str());
}

TEST(GISEx1, loadMap2SaveClearAndLoad) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"ex1_map2.json"};
	auto loaded_input = gis.loadMapFile(input_file);
	std::sort(loaded_input.begin(), loaded_input.end());
	std::string output_file{"out."+input_file};
	std::size_t saved = gis.saveMapFile(output_file);
	std::size_t number_of_cleared_entities = gis.clear();
	EXPECT_TRUE(number_of_cleared_entities == saved && saved == getInputFileEntities(input_file).size());
	auto loaded_output = gis.loadMapFile(output_file);
	std::sort(loaded_output.begin(), loaded_output.end());
	EXPECT_EQ(loaded_input, loaded_output);
	std::remove(output_file.c_str());
}

TEST(GISEx1, twoLoadsMap1Map2ClearSaveAndLoad) {
    	auto gis = GISProvider::getGIS();
	std::string input_file1{"ex1_map1.json"};
	std::vector<EntityId> loaded_entities1 = gis.loadMapFile(input_file1);
	std::string input_file2{"ex1_map2.json"};
	std::vector<EntityId> loaded_entities2 = gis.loadMapFile(input_file2);
	std::string output_file{"out_ex1_map1map2.json"};
	std::size_t number_of_saved_entities = gis.saveMapFile(output_file);
	std::size_t number_of_cleared_entities = gis.clear();
	EXPECT_TRUE(getInputFileEntities(input_file1).size() + getInputFileEntities(input_file2).size() == number_of_saved_entities
	&& number_of_saved_entities == number_of_cleared_entities);
	std::vector<EntityId> loaded_entities = gis.loadMapFile(output_file);
	std::vector<EntityId> combined_entities = getCombinedFilesEntities(input_file1, input_file2);
	std::sort(combined_entities.begin(), combined_entities.end());
	std::sort(loaded_entities.begin(), loaded_entities.end());
	EXPECT_EQ(loaded_entities, combined_entities);
	std::remove(output_file.c_str());
}

TEST(GISEx1, loadNonExistantFile) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"___non_existant_file___"};
	std::vector<EntityId> loaded_entities = gis.loadMapFile(input_file);
	std::size_t expected_number_of_loaded_entities = 0;
	EXPECT_EQ(loaded_entities.size(), expected_number_of_loaded_entities);
}

TEST(GISEx1, loadMapFileTwoWays) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"ex1_two_ways.json"};
    	auto loaded_entities = gis.loadMapFile(input_file); 
	EXPECT_EQ(loaded_entities, getInputFileEntities(input_file));
}

TEST(GISEx1, getWayClosestPointTwoWays1) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"ex1_two_ways.json"};
    	gis.loadMapFile(input_file); 
	Coordinates point{Longitude{47}, Latitude{30.312}}; 
	auto closest_way = gis.getWayClosestPoint(point);
	auto closest_point = CoordinatesMath::closestPointOnSegment(point, ex1_two_ways_LeftWay.from, ex1_two_ways_LeftWay.to);
	EXPECT_EQ(std::get<AbstractGIS::ClosestPoint::Coord>(closest_way), closest_point);
	EXPECT_EQ(std::get<AbstractGIS::ClosestPoint::WayId>(closest_way), EntityId{"LeftWay"});
}

TEST(GISEx1, loadMapFileTwoWays180thMeridian) {
    	auto gis = GISProvider::getGIS();
	std::string input_file{"ex1_two_ways_180th_meridian.json"};
    	auto loaded_entities = gis.loadMapFile(input_file); 
	EXPECT_EQ(loaded_entities, getInputFileEntities(input_file));
}

TEST(GISEx2, getEntityClosestPoint1) {
    	auto gis = GISProvider::getGIS();
    	gis.loadMapFile("ex1_map1.json");
	Coordinates point{Longitude{40.0}, Latitude{10.0}};
	Coordinates pnt_center{Longitude{20.0}, Latitude{70.0}};
	Meters pnt_radius{1001.96};
	std::optional<Coordinates> closest_point = gis.getEntityClosestPoint(EntityId{"Pnt1"}, point);
	Coordinates expected_point = CoordinatesMath::closestPointOnCircle(point, pnt_center, pnt_radius);
	ASSERT_TRUE(closest_point);
	EXPECT_EQ(*closest_point, expected_point);
}

TEST(GISEx2, getEntityClosestPoint2) {
    	auto gis = GISProvider::getGIS();
    	gis.loadMapFile("ex1_map2.json");
	Coordinates point{Longitude{-1}, Latitude{-1}};
	Coordinates pnt_center{Longitude{20.730811}, Latitude{10.997455}};
	Meters pnt_radius{100.96};
	std::optional<Coordinates> closest_point = gis.getEntityClosestPoint(EntityId{"P1"}, point);
	Coordinates expected_point = CoordinatesMath::closestPointOnCircle(point, pnt_center, pnt_radius);
	ASSERT_TRUE(closest_point);
	EXPECT_EQ(*closest_point, expected_point);
}


