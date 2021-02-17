#include "GIS.h"
//constexpr size_t len_rand_str = 10;

REGISTER_GIS(GIS_313452542)

using namespace rapidjson;


//------------------------clear---------------------
std::size_t GIS_313452542::clear() {
	std::size_t num_of_deletions = idEntityMap.size();
	idEntityMap.clear();
	juncidCoorMap.clear();
	nameIDvecMap.clear();
	grid.clear();
	grid_way.clear();
	juncId_waysId_map.clear();
	return num_of_deletions;
}


//--------------------loadMapFile-------------------
std::vector<EntityId> GIS_313452542::loadMapFile(const std::string& filename) {
	std::vector <EntityId> id_vector;
	std::vector<std::shared_ptr<Entity>> entities;

	Document document;
	createDocument(filename, document);
	createEntityVector(document, entities);
	loadEntities(entities, id_vector);
	return id_vector;
}
//  Parse a JSON text string to a document return 0 if success and else return 1;
int GIS_313452542::createDocument(const std::string& filename, rapidjson::Document& document) {
	std::string error;
	const char* input_file = filename.c_str();
	std::ifstream ifile(input_file, std::ios::ate); //create ifile and move position to the end
	if (!ifile) {
		error = (std::string)"Could not open file" + (std::string)input_file;
		printError(error);

		return 1;
	}
	std::streamsize size = ifile.tellg(); // return the current position(in the end)
	ifile.seekg(0, std::ios::beg); // position back to begining

	std::vector<char> buffer(size + 1);

	if (!ifile.read(buffer.data(), size)) {
		error = (std::string)"Could not read file " + (std::string)input_file;
		printError(error);
		return 1;
	}
	buffer[size] = '\0';

	ifile.close();

	ParseResult ok = document.Parse(buffer.data());
	if (!ok) {
		error = "failed to parse";
		std::cerr << "JSON Parse: " << ok.Code() << " " << ok.Offset() << "\n";

		printError(error);

		return 1;
	}
	return 0;
}

//print error msg to screen. need modify to error.log 
void GIS_313452542::printError(std::string error) const {
	std::ofstream myfile;
	myfile.open("errors.log", std::ofstream::in |
		std::ofstream::out | std::ofstream::app);
	myfile << error << std::endl;
	myfile.close();
}

void GIS_313452542::loadEntities(std::vector<std::shared_ptr<Entity>>& entitys, std::vector<EntityId>& idVec) {
	std::vector<size_t> idLessEntities;
	for (size_t i = 0; i < entitys.size(); i++) {
		EntityId id = entitys[i]->getEntityId();
		if (static_cast<std::string>(id) != "") {
			idVec.push_back(id);
			insertEntityToGIS(entitys[i]);
		}
		else {
			idVec.push_back(EntityId(""));
			idLessEntities.push_back(i);
		}
	}
	for (const auto& i : idLessEntities) {
		EntityId randId = EntityId(random_string(len_rand_str));
		while (idEntityMap.findEntity(randId)) {
			randId = EntityId(random_string(len_rand_str));
		}
		entitys[i]->setID(randId);
		insertEntityToGIS(entitys[i]);
		idVec[i] = randId;
	}
}

//generates a random string
std::string GIS_313452542::random_string(size_t length) const {
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

//insert &ent to the GIS object
bool GIS_313452542::insertEntityToGIS(std::shared_ptr<Entity>& ent) {
	if (ent->getType() == Entity::EntityType::Junction) {
		grid.insert(*(dynamic_cast<Junction*>(ent.get())));
	}
	else if (ent->getType() == Entity::EntityType::POI) {
		grid.insert(*(dynamic_cast<POI*>(ent.get())));
	}
	else {
		auto itr_from = juncidCoorMap.find(dynamic_cast<Way*>(ent.get())->getFromEntityId());
		auto itr_to = juncidCoorMap.find(dynamic_cast<Way*>(ent.get())->getToEntityId());
		if ((itr_from != juncidCoorMap.end()) && (itr_to != juncidCoorMap.end())) {
			Coordinates from = juncidCoorMap.at(itr_from->first);
			Coordinates to = juncidCoorMap.at(itr_to->first);
			Way* way = dynamic_cast<Way*>(ent.get());
			grid.insert(*way, from, to);
			grid_way.insert(*way, from, to);
			insertTojuncIdWaysIDMap(way->getFromEntityId(), way->getEntityId());
			if (way->isBidirectional()) {
				insertTojuncIdWaysIDMap(way->getToEntityId(), way->getEntityId());
			}
		}
		else {
			printError("Bad way, from and to are not in the GIS");
			return false;
		}
	}
	const std::string& name = (ent->getEntityName());
	const EntityId id = ent->getEntityId();
	auto srchIdEnt = idEntityMap.findEntity(id);
	auto srchNameId = nameIDvecMap.find(name);
	if (srchIdEnt == nullptr) {//not found
		insertToNameIdvecMap(srchNameId, name, id);
	}
	else {
		srchNameId->second.erase(std::remove(	srchNameId->second.begin(),
												srchNameId->second.end(), id));//worse case O(n)
		
		insertToNameIdvecMap(srchNameId, name, id);
	}
	insertToIdEntityMap(ent);
	return true;
}

void GIS_313452542::insertTojuncIdWaysIDMap(const EntityId& juncID, const EntityId& wayID) {
	auto itr_junc = juncId_waysId_map.find(juncID);
	if (itr_junc != juncId_waysId_map.end()) {
		itr_junc->second.insert(wayID);
	}
	else {
		juncId_waysId_map.insert({ juncID, std::unordered_set<EntityId,IDhash>({wayID}) });
	}
}


//insert &ent to idEntityMap according to ent type
void GIS_313452542::insertToIdEntityMap(std::shared_ptr<Entity> ent) {
	idEntityMap.insert(ent);
}

//insert pair {name, id} to nameIDvecMap according to the iterator seearch
void GIS_313452542::insertToNameIdvecMap(const std::unordered_map<std::string, std::vector<EntityId>>::iterator& search, const std::string& name, const EntityId id) {
	if (search == nameIDvecMap.end()) {//not found
		nameIDvecMap.insert({ name, std::vector<EntityId>(1,id) });
	}
	else {
		nameIDvecMap.at(name).push_back(id);
	}
}



bool GIS_313452542::validCoordinates(Value::ConstValueIterator itr, std::string type, std::string name) const {
	if (itr->HasMember("coordinates")) {
		if ((*itr)["coordinates"].IsArray()) {
			if ((*itr)["coordinates"].Size() != 2) {
				printError("Bad Junction, " + name + ": coordinates need to be an Array of size 2");
				return false;
			}
			for (SizeType i = 0; i < (*itr)["coordinates"].Size(); i++) {
				if (!(*itr)["coordinates"][i].IsNumber()) {
					printError("Bad " + type + ", " + name + "coordinates need to be an Array of Double");
					return false;
				}
			}
		}
		else {
			printError("Bad " + type + ", " + name + " coordinates need to be an Array ###");
			return false;
		}
	}
	else {
		printError("Bad " + type + ", " + name + ": not exist coordinates key");
		return false;
	}
	return  true;
}

void GIS_313452542::insertJunction(std::vector<std::shared_ptr<Entity>>& entities, const Value::ConstValueIterator& itr,
	const std::string& id, const std::string& name,
	const std::string& description, const std::set<std::string>& category_tags) {

	if (validCoordinates(itr, "Junction", name)) {

		Longitude lon = Longitude((*itr)["coordinates"][0].GetDouble());
		Latitude lat = Latitude((*itr)["coordinates"][1].GetDouble());
		Coordinates coordinates = Coordinates(lon, lat);
		entities.push_back(std::make_shared<Junction>(EntityId(id), name, description, category_tags, coordinates));
		juncidCoorMap.insert({ EntityId(id),coordinates });
	}
}


bool GIS_313452542::validFrom(Value::ConstValueIterator itr, std::string type, std::string name) const {
	if (itr->HasMember("from")) {
		if ((*itr)["from"].IsObject()) {
			if ((*itr)["from"].HasMember("entity_id")) {
				if (!(*itr)["from"]["entity_id"].IsString()) {
					printError("Bad " + type + ", " + name + " : entity id in from is not a string");
					return false;
				}
			}
			else {
				printError("Bad " + type + ", " + name + " : doesnt exist entity_id in from");
				return false;
			}
		}
		else {
			printError("Bad Way, " + name + ": from is not object");
			return false;
		}
	}
	else {
		printError("Bad Way, " + name + ": not exist from key");
		return false;
	}
	return true;
}
bool GIS_313452542::validTo(Value::ConstValueIterator itr, std::string type, std::string name)  const {
	if (itr->HasMember("to")) {
		if ((*itr)["to"].IsObject()) {
			if ((*itr)["to"].HasMember("entity_id")) {
				if (!(*itr)["to"]["entity_id"].IsString()) {
					printError("Bad " + type + ", " + name + " : entity id in to is not a string");
					return false;
				}
			}
			else {
				printError("Bad " + type + ", " + name + " : doesnt exist entity_id in to");
				return false;
			}
		}
		else {
			printError("Bad Way, " + name + ": to is not object");
			return false;
		}
	}
	else {
		printError("Bad Way, " + name + ": not exist to key");
		return false;
	}
	return true;
}

bool GIS_313452542::validDirection(Value::ConstValueIterator itr, std::string name) const {
	if (itr->HasMember("direction")) {
		if (!(*itr)["direction"].IsString()) {
			printError("Bad Way, " + name + "direction need to be a String");
			return false;
		}
		std::string direction = (*itr)["direction"].GetString();
		if ((direction.compare("bidirectional") != 0) && (direction.compare("unidirectional") != 0)) {
			printError("Bad Way, " + name + ": direction need to be bidirectional or unidirectional");
			return false;
		}
	}
	else {
		printError("Bad Way, " + name + ": not exist direction key");
		return false;
	}
	return true;
}

bool GIS_313452542::validSpeedLimit(Value::ConstValueIterator itr, std::string name) const {
	if (itr->HasMember("speed_limit")) {
		if (!(*itr)["speed_limit"].IsNumber()) {
			printError("Bad Way, " + name + ": speed_limit need to be a double");
			return false;
		}
	}
	else {
		printError("Bad Way, " + name + ": not exist speed_limit key");
		return false;
	}
	return true;
}

bool GIS_313452542::validtollRoad(Value::ConstValueIterator itr, std::string name)  const {
	if (itr->HasMember("toll_road")) {
		if (!(*itr)["toll_road"].IsBool()) {
			printError("Bad Way, " + name + ": tool_road need to be boolean");
			return false;
		}
	}
	else {
		printError("Bad Way, " + name + ": not exist toll_road key");
		return false;
	}
	return true;
}

bool GIS_313452542::validCurves(Value::ConstValueIterator itr, std::string name) const {
	if (!(*itr)["curves"].IsArray()) {
		printError("Bad Way, " + name + ": curves need to be an Array");
		return false;
	}
	for (SizeType i = 0; i < (*itr)["curves"].Size(); i++) {
		if ((*itr)["curves"].GetArray()[i].Size() != 2) {
			printError("Bad Way, " + name + ": coordinates in curves need to be in size 2");
			return false;
		}
		for (SizeType j = 0; j < (*itr)["curves"][i].Size(); j++) {

			if (!(*itr)["curves"][i][j].IsNumber()) {
				printError("Bad Way, " + name + ": curves ned to be an Array of Array of double");
				return false;
			}
		}

	}


	return true;
}

bool GIS_313452542::validRestricted(Value::ConstValueIterator itr, std::string name) const {
	if (!(*itr)["restricted"].IsArray()) {
		printError("Bad Way, " + name + ": restricted need to be an Array");
		return false;
	}
	for (SizeType i = 0; i < (*itr)["restricted"].Size(); i++) {
		if (!(*itr)["restricted"][i].IsString()) {
			printError("Bad Way, " + name + ": restricted need to be an Array of String");
			return false;
		}
	}

	return true;
}


void GIS_313452542::insertWay(std::vector<std::shared_ptr<Entity>>& entities, const Value::ConstValueIterator& itr,
	const std::string& id, const std::string& name, const std::string& description,
	const std::set<std::string>& category_tags) {

	Longitude lon = Longitude(0);
	Latitude lat = Latitude(0);
	Coordinates coordinates = Coordinates(lon, lat);
	std::vector <Coordinates> curves;
	std::vector<std::string> restricted;
	if (validFrom(itr, "Way", name) &&
		validTo(itr, "Way", name) &&
		validDirection(itr, name) &&
		validSpeedLimit(itr, name) && 
		validtollRoad(itr, name))
	{
		const std::string from = (*itr)["from"]["entity_id"].GetString();
		const std::string to = (*itr)["to"]["entity_id"].GetString();
		std::string direction = (*itr)["direction"].GetString();
		int speed_limit = (*itr)["speed_limit"].GetInt();
		bool toll_road = (*itr)["toll_road"].GetBool();

		if (itr->HasMember("curves")) {
			if (validCurves(itr, name)) {
				for (SizeType i = 0; i < (*itr)["curves"].Size(); i++) {
					lon = Longitude((*itr)["curves"][i][0].GetDouble());
					lat = Latitude((*itr)["curves"][i][1].GetDouble());
					coordinates = Coordinates(lon, lat);
					curves.push_back(coordinates);
				}
			}
			else return;
		}
		if (itr->HasMember("restricted")) {
			if (validRestricted(itr, name)) {
				for (SizeType i = 0; i < (*itr)["restricted"].Size(); i++) {
					restricted.push_back((*itr)["restricted"][i].GetString());
				}
			}
			else return;
		}
		entities.push_back(std::make_shared<Way>(EntityId(id), name, description, category_tags,
			EntityId(from), EntityId(to), direction, speed_limit, toll_road, curves, restricted));
	}
}

bool GIS_313452542::validRadius(Value::ConstValueIterator itr, std::string name) const {
	if ((*itr)["geometry"].HasMember("radius")) {
		if (!(*itr)["geometry"]["radius"].IsNumber()) {
			printError("Bad POI, " + name + ": radius need to be a double");
			return false;
		}
	}
	else {
		printError("Bad POI, " + name + ": not exist radius key");
		return false;
	}
	return true;
}

bool GIS_313452542::validGeomtry(Value::ConstValueIterator itr, std::string name) const {
	if (validTypeGeomtry(itr, name) && validCircleCoor(itr, name) && validRadius(itr, name)) {
		return true;
	}
	return false;
}

bool GIS_313452542::validCircleCoor(Value::ConstValueIterator itr, std::string name) const {
	if ((*itr)["geometry"].HasMember("coordinates")) {
		if ((*itr)["geometry"]["coordinates"].IsArray()) {
			if ((*itr)["geometry"]["coordinates"].GetArray().Size() != 2) {
				printError("Bad POI, " + name + ": coordinates need to be an array of size 2");
				return false;
			}
			for (SizeType i = 0; i < (*itr)["geometry"]["coordinates"].Size(); i++) {

				if (!(*itr)["geometry"]["coordinates"][i].IsNumber()) {
					printError("Bad POI, " + name + ": coordinates need to be an Array of double");
					return false;
				}
			}
		}
		else {
			printError("Bad POI, " + name + ": coordinates in geometry need to be Array");
			return false;
		}

	}
	else {
		printError("Bad POI, " + name + ": not exist coordinates key");
		return false;
	}
	return true;
}

bool GIS_313452542::validAccessibility(Value::ConstValueIterator itr, std::string name) const {
	for (SizeType i = 0; i < (*itr)["accessibility"].Size(); i++) {
		if (!(*itr)["accessibility"][i].IsString()) {
			printError("Bad POI, " + name + ": accessibility need to be Array o string");
			return false;
		}
	}

	return true;
}

bool GIS_313452542::validTypeGeomtry(Value::ConstValueIterator itr, std::string name)  const {
	if ((*itr)["geometry"].HasMember("type")) {
		if (!(*itr)["geometry"]["type"].IsString()) {
			printError("Bad POI, " + name + ": type in geometry need to be String");
			return false;
		}
		std::string type = (*itr)["geometry"]["type"].GetString();
		if (type.compare("Circle") != 0) {
			printError("Bad POI, " + name + ": type in geometry need to be Circle");
			return false;
		}
	}
	else {
		printError("Bad POI, " + name + ": not exist type key");
		return false;
	}
	return true;
}

void GIS_313452542::insertPOI(std::vector<std::shared_ptr<Entity>>& entities, const Value::ConstValueIterator& itr,
	const std::string& id, const std::string& name, const std::string& description,
	const std::set<std::string>& category_tags) {

	std::vector<std::string> accessibility;

	if (validGeomtry(itr, name)) {
		Longitude lon = Longitude((*itr)["geometry"]["coordinates"][0].GetDouble());
		Latitude lat = Latitude((*itr)["geometry"]["coordinates"][1].GetDouble());
		Coordinates coordinates = Coordinates(lon, lat);

		double radius = (*itr)["geometry"]["radius"].GetDouble();
		Circle circle = Circle(Meters(radius), coordinates);
		if (itr->HasMember("accessibility")) {
			if (validAccessibility(itr, name)) {
				for (SizeType i = 0; i < (*itr)["accessibility"].Size(); i++) {
					accessibility.push_back((*itr)["accessibility"][i].GetString());
				}
			}
			else return;
		}

		entities.push_back(std::make_shared<POI>(EntityId(id), name, description, category_tags, circle, accessibility));
	}
}

// check if the category_tag field is valid if exist 
bool GIS_313452542::validCategoryTag(Value::ConstValueIterator itr, std::string type, std::string name, std::set<std::string>& category_tags) const {
	if (itr->HasMember("category_tags")) {
		if (!(*itr)["category_tags"].IsArray()) {
			printError("Bad " + type + ", " + name + " description value need to be an Array");
			return false;
		}
		else {
			for (SizeType i = 0; i < (*itr)["category_tags"].Size(); i++) {
				if (!(*itr)["category_tags"][i].IsString()) {
					printError("Bad " + type + ", " + name + " description value need to be an Array of String");
					return false;
				}
				else category_tags.insert((*itr)["category_tags"].GetString());
			}
		}
	}
	return true;
}
// check if the description field is valid if exist 
bool GIS_313452542::validDescription(Value::ConstValueIterator itr, std::string type, std::string name, std::string& description) const {
	if (itr->HasMember("description")) {
		if (!(*itr)["description"].IsString()) {
			printError("Bad " + type + ", " + name + ": description value need to be a String");
			return false;
		}
		description = (*itr)["description"].GetString();

	}
	return true;
}
// check if the name field is valid (need to be exist) 
bool GIS_313452542::validName(Value::ConstValueIterator itr, std::string type)  const {
	if (itr->HasMember("name")) {
		if ((*itr)["name"].IsString()) {
			return true;
		}
		else printError("Bad " + type + ": name value need to be a String");
	}
	else printError("Bad " + type + ": not exist name key");
	return false;
}

// check if the id field is valid if exist 
bool GIS_313452542::validId(Value::ConstValueIterator itr, std::string type, std::string name, std::string& id) const {
	if (itr->HasMember("id")) {
		if (!(*itr)["id"].IsString()) {
			printError("Bad " + type + ", " + name + ": id value need to be a String");
			return false;
		}
		id = (*itr)["id"].GetString();
		if (idEntityMap.findEntity(EntityId(id)) != nullptr) {
			printError("Bad entity, id: " + id + " already exists");
			return false;
		}
	}
	return true;
}
// check if the category_tag field is valid (need to be exist) 
bool GIS_313452542::validType(Value::ConstValueIterator itr)  const {
	if (itr->HasMember("type")) {
		if ((*itr)["type"].IsString()) {
			std::string type = (*itr)["type"].GetString();
			if ((type.compare("Junction") == 0) || (type.compare("POI") == 0) || (type.compare("Way") == 0)) {
				return true;
			}
			else printError("Bad Entity: type value need to be Junction, POI or Way");
		}
		else printError("Bad Entity: type value need to be a string");
	}
	else printError("Bad Entity: not exist type key");

	return false;
}

void GIS_313452542::createEntityVector(rapidjson::Document& document, std::vector<std::shared_ptr<Entity>>& entities) {

	for (Value::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr) {
		std::string id = "";
		std::string description = "";
		std::set<std::string> category_tags;
		if (itr->IsObject()) {
			if (validType(itr)) {
				const char* type = (*itr)["type"].GetString();
				if (validName(itr, type)) {
					std::string name = (*itr)["name"].GetString();
					if (validId(itr, type, name, id) &&
						validDescription(itr, type, name, description) &&
						validCategoryTag(itr, type, name, category_tags))
					{
						if (strcmp(type, "Junction") == 0) {
							insertJunction(entities, itr, id, name, description, category_tags);
						}
						else if (strcmp(type, "Way") == 0) {
							insertWay(entities, itr, id, name, description, category_tags);
						}
						else if (strcmp(type, "POI") == 0) {
							insertPOI(entities, itr, id, name, description, category_tags);
						}
					}
				}
			}
		}
		else printError("Bad Entity: entity need to be an object");
	}

}



//--------------------saveMapFile---------------

std::size_t GIS_313452542::saveMapFile(const std::string& filename) {

	std::size_t countEntity = 0;
	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);

	std::vector<Junction> junctions;
	std::vector<POI> pois;
	std::vector<Way> ways;
	getJunctions(junctions);
	getPOIs(pois);
	getWays(ways);


	writer.StartArray();
	for (const auto& junc : junctions) {
		writer.StartObject();
		insertJunctionObject(junc, writer);
		countEntity++;
		writer.EndObject();
	}
	for (const auto& poi : pois) {
		writer.StartObject();
		insertPOIObject(poi, writer);
		countEntity++;
		writer.EndObject();
	}
	for (const auto& way : ways) {
		writer.StartObject();
		insertWayObject(way, writer);
		countEntity++;
		writer.EndObject();
	}
	writer.EndArray();
	std::ofstream ofile(filename);
	if (!ofile) {
		printError("Could not open file " + filename);
		return EXIT_FAILURE;
	}

	ofile << sb.GetString();
	ofile.close();
	return countEntity;
}

void GIS_313452542::getJunctions(std::vector<Junction>& junctions) {
	idEntityMap.getJunctions(junctions);
}

void GIS_313452542::getWays(std::vector<Way>& ways) {
	idEntityMap.getWays(ways);
}
void GIS_313452542::getPOIs(std::vector<POI>& pois) {
	idEntityMap.getPOIs(pois);
}

void GIS_313452542::insertKeyString(const char* key, std::string val, PrettyWriter<StringBuffer>& writer) {
	writer.Key(key);
	writer.String(val.c_str(), (SizeType)val.length());
}

void GIS_313452542::insertSharedField(const std::unique_ptr<Entity> ent, PrettyWriter<StringBuffer>& writer) {
	insertKeyString("id", ent->getEntityId(), writer);
	insertKeyString("name", ent->getEntityName(), writer);
	insertKeyString("description", ent->getEntityDescription(), writer);
	std::set<std::string> category_tag = ent->getEntityCategoryTag();
	writer.Key("category_tag");
	writer.StartArray();
	for (std::string tag : category_tag) {
		writer.String(tag.c_str(), (SizeType)tag.length());
	}
	writer.EndArray();
}

void GIS_313452542::insertJunctionObject(const Junction& junc, PrettyWriter<StringBuffer>& writer) {
	insertKeyString("type", "Junction", writer);
	insertSharedField(std::make_unique<Entity>(junc), writer);
	writer.Key("coordinates");
	writer.StartArray();
	writer.Double((double)junc.getLongitude());
	writer.Double((double)junc.getLatitude());
	writer.EndArray();
}

void GIS_313452542::insertWayObject(const Way& way, PrettyWriter<StringBuffer>& writer) {
	insertKeyString("type", "Way", writer);
	insertSharedField(std::make_unique<Entity>(way), writer);
	writer.Key("from");
	writer.StartObject();
	insertKeyString("entity_id", way.getFromEntityId(), writer);
	writer.EndObject();
	writer.Key("to");
	writer.StartObject();
	insertKeyString("entity_id", way.getToEntityId(), writer);
	writer.EndObject();
	
	const std::vector<Coordinates>& curves = way.getCurves();
	if (curves.size() > 0) {
		writer.Key("curves");
		writer.StartArray();
		for (const auto& curve : curves) {
			writer.StartArray();
			writer.Double((double)curve.longitude());
			writer.Double((double)curve.latitude());
			writer.EndArray();
		}
		writer.EndArray();
	}
	insertKeyString("direction", way.getDirection(), writer);
	writer.Key("speed_limit");
	writer.Int(way.getSpeedLimit());
	writer.Key("toll_road");
	writer.Bool(way.isToll());

	std::vector<std::string> restricted = way.getRestricted();
	if (!restricted.empty()) {
		writer.Key("restricted");
		writer.StartArray();
		for (std::string restrict : restricted) {
			writer.String(restrict.c_str(), (SizeType)restrict.length());
		}
		writer.EndArray();
	}
}

void GIS_313452542::insertPOIObject(const POI& poi, PrettyWriter<StringBuffer>& writer) {

	insertKeyString("type", "POI", writer);
	insertSharedField(std::make_unique<Entity>(poi), writer);

	const std::vector<std::string>& accessibilty = poi.getAccessibility();
	writer.Key("accessibilty");
	writer.StartArray();
	for (const auto& access : accessibilty) {
		writer.String(access.c_str(), (SizeType)access.length());
	}
	writer.EndArray();
	writer.Key("geometry");
	writer.StartObject();
	insertKeyString("type", "Circle", writer);
	writer.Key("coordinates");
	writer.StartArray();
	const Circle& c = poi.getShape();
	writer.Double((double)c.getCoordinates().longitude());
	writer.Double((double)c.getCoordinates().latitude());
	writer.EndArray();
	writer.Key("radius");
	writer.Double((double)c.getRadius());
	writer.EndObject();
}


//-------------------------getEntities----------------

std::vector<EntityId> GIS_313452542::getEntities(const std::string& search_name)const {
	std::vector <EntityId> id_vec;
	if (nameIDvecMap.find(search_name) != nameIDvecMap.end()) {
		for (const auto& id : nameIDvecMap.at(search_name)) {
			id_vec.push_back(id);
		}
	}
	return id_vec;
}

std::vector<EntityId> GIS_313452542::getEntities(const std::string& search_name,
	const Coordinates& coordinates, Meters radius) {


	std::vector<EntityId> id_vector;
	std::vector<EntityId> tmp_vector;
	std::unordered_set<EntityId> memory_set;
	

	grid.searchInTop(	coordinates,
						radius,
						search_name,
						tmp_vector,
						memory_set);

	grid.searchInBottom(coordinates,
						radius,
						search_name,
						tmp_vector,
						memory_set);
	
	for (const auto& id : tmp_vector) {
		std::shared_ptr<Entity> p_ent = idEntityMap.findEntity(id);

		if (isEntityInCircle(p_ent, coordinates, radius)) {
			id_vector.push_back(id);
		}
	}

	return id_vector;
	
}

bool GIS_313452542::isEntityInCircle(const std::shared_ptr<Entity> p_ent, const Coordinates& center, Meters radius) const {

	if (p_ent->getType() == Entity::EntityType::Junction) {
		Junction* junc = dynamic_cast<Junction*>(p_ent.get());
		return isJunctionInCircle(junc, center, radius);
	}
	else if (p_ent->getType() == Entity::EntityType::POI) {
		POI& poi = (*dynamic_cast<POI*>(p_ent.get()));
		return isPOIInCircle(poi, center, radius);
	}
	else if (p_ent->getType() == Entity::EntityType::Way) {
		Way* way = dynamic_cast<Way*>(p_ent.get());
		return isWayInCircle(way, center, radius);
	}
	return false;
}

bool GIS_313452542::isJunctionInCircle(const Junction* junc, const Coordinates& center, Meters radius) const {
	Meters dist_from_center = CoordinatesMath::calculateDistance(junc->getCoordinates(), center);
	return (dist_from_center <= radius);
}

bool GIS_313452542::isPOIInCircle(const POI& poi, const Coordinates& center, Meters radius) const {
	Coordinates closest_poi_coor = getPOIClosestPoint(poi, center);
	Meters dist_from_center = CoordinatesMath::calculateDistance(closest_poi_coor, center);
	return (dist_from_center <= radius);
}

bool GIS_313452542::isWayInCircle(const Way* way, const Coordinates& center, Meters radius) const {
	std::vector<Coordinates> curves = way->getCurves();
	Coordinates to = idEntityMap.findJunction(way->getToEntityId())->second.getCoordinates();
	Coordinates from = idEntityMap.findJunction(way->getFromEntityId())->second.getCoordinates();

	if (curves.size() > 0) {
		auto coor_dist = CoordinatesMath::closestPointOnSegmentAndDistance(center, from, curves.at(0));
		Meters dist_from_center = coor_dist.second;
		if (dist_from_center <= radius) { return true; }

		coor_dist = CoordinatesMath::closestPointOnSegmentAndDistance(center, curves.at(curves.size() - 1), to);
		dist_from_center = coor_dist.second;
		if (dist_from_center <= radius) { return true; }

		for (auto iter = curves.begin(); iter != curves.end(); iter++) {
			auto next_iter = iter + 1;
			if (next_iter != curves.end()) {
				coor_dist = CoordinatesMath::closestPointOnSegmentAndDistance(center, *iter, *next_iter);
				dist_from_center = coor_dist.second;
				if (dist_from_center <= radius) { return true; }
			}
			else { break; }
		}
	}
	else {//curves size = 0
		auto coor_dist = CoordinatesMath::closestPointOnSegmentAndDistance(center, from, to);
		Meters dist_from_center = coor_dist.second;
		if (dist_from_center <= radius) { return true; }
	}
	return false;
}

//-------------------getWayClosestPoint------------------

std::tuple<Coordinates, EntityId, std::size_t> GIS_313452542::getWayClosestPoint(const Coordinates& coordinates) const {
	Meters radius = grid_way.precisionInMeters();
	std::vector<EntityId> waysId_vec;
	Way way = idEntityMap.beginWay()->second;
	EntityId closest_id = way.getEntityId();
	auto pair_coords_segment = getWayClosestPoint(way, coordinates);
	Meters closest_dist = CoordinatesMath::calculateDistance(coordinates, pair_coords_segment.first);
	int i = num_of_iterations;

	while (--i >= 0 && radius <= CoordinatesMath::earth_radius) {
		getWays(coordinates, radius, waysId_vec);
		if (waysId_vec.size() > 0) {
			for (auto itr = waysId_vec.begin(); itr != waysId_vec.end(); itr++) {
				checkIfCloser(coordinates, *itr, pair_coords_segment, closest_id, closest_dist);
			}			
			return { pair_coords_segment.first, closest_id , pair_coords_segment.second };
		}
		else {
			radius = radius + grid_way.precisionInMeters();
		}
	}
	for (const auto& pr : idEntityMap.id_way_map) {
		checkIfCloser(coordinates, pr.first, pair_coords_segment, closest_id, closest_dist);
	}
	return { pair_coords_segment.first, closest_id, pair_coords_segment.second };
}

std::tuple<Coordinates, EntityId, std::size_t> GIS_313452542::getWayClosestPoint(
	const Coordinates& coordinates, const Restrictions& res) const {

	Meters radius = grid_way.precisionInMeters();
	std::vector<EntityId> waysId_vec;
	EntityId closest_id = EntityId("");
	std::pair<Coordinates, std::size_t> pair_coords_segment = std::make_pair(Coordinates(Longitude(Way::defult_longi), Latitude(Way::defult_lati)), 0);
	Meters closest_dist = Meters(1e9);

	int i = num_of_iterations;
	while (--i >= 0 && radius <= CoordinatesMath::earth_radius) {
		getWays(coordinates, radius, waysId_vec);
		removeRestrictedWays(waysId_vec, res);
		if (waysId_vec.size() > 0) {
			for (auto itr = waysId_vec.begin(); itr != waysId_vec.end(); itr++) {
				if (isWayNOTRestricted(*itr, res)) {
					checkIfCloser(coordinates, *itr, pair_coords_segment, closest_id, closest_dist);
				}
			}
			return { pair_coords_segment.first, closest_id, pair_coords_segment.second };
		}
		else {
			radius = radius + grid_way.precisionInMeters();
		}
	}
	for (const auto& pr : idEntityMap.id_way_map) {
		pr.second.print();
		if (isWayNOTRestricted(pr.first, res)) {
			checkIfCloser(coordinates, pr.first, pair_coords_segment, closest_id, closest_dist);
		}
	}

	if (std::string("").compare((std::string)closest_id) == 0) {

		return getWayClosestPoint(coordinates);
	}
	return { pair_coords_segment.first, closest_id, pair_coords_segment.second };
}

void GIS_313452542::removeRestrictedWays(std::vector<EntityId>& waysId, const Restrictions& res) const {

	auto it = waysId.begin();
	while (it != waysId.end()) {
		if (isWayRestricted(*it, res)) {
			it = waysId.erase(it);
		}
		else {
			++it;
		}
	}
}

bool GIS_313452542::isWayNOTRestricted(const EntityId& way_id, const Restrictions& res) const {
	return !isWayRestricted(way_id, res);
}

bool GIS_313452542::isWayRestricted(const EntityId& way_id, const Restrictions& res) const {
	const Way& way = idEntityMap.id_way_map.at(way_id);
	return ((res.contains("toll") && way.isToll()) || (res.contains("highway") && way.isHighway()));
}

void GIS_313452542::getWays(const Coordinates& coordinates, const Meters radius, std::vector<EntityId>& waysId_vec) const {
	std::unordered_set<EntityId> memory_set;

	grid_way.searchInTop(	coordinates,
							radius,
							DEFAULT_WAY_NAME,
							waysId_vec,
							memory_set);

	grid_way.searchInBottom(coordinates,
							radius,
							DEFAULT_WAY_NAME,
							waysId_vec,
							memory_set);		
}

void GIS_313452542::checkIfCloser(	const Coordinates& coordinates,
									const EntityId& id,
									std::pair<Coordinates, std::size_t>& pair_coords_segment,
									EntityId& closest_id,
									Meters& closest_dist) const 
{
	const Way& way = idEntityMap.findWay(id)->second;
	auto tmp_coords_sigment = getWayClosestPoint(way, coordinates);
	Meters tmp_dist = CoordinatesMath::calculateDistance(coordinates, tmp_coords_sigment.first);
	if (tmp_dist < closest_dist) {
		closest_dist = tmp_dist;
		closest_id = id;
		pair_coords_segment = tmp_coords_sigment;

	}
}


//---------prints-------------------


void GIS_313452542::printEntityIDMap() const {
	idEntityMap.print();
}
void GIS_313452542::printNameIdVecMap() const {
	for (auto const& pr : nameIDvecMap) {
		std::cout << "name: " << pr.first << " , id-vector: " << std::endl;
		for (auto const& it : pr.second)
			std::cout << "\t" << (std::string)it << std::endl;
	}
}
void GIS_313452542::printGrid() const {
	//std::cout << "\n -------------- grid --------------\n";
	grid.printGrid();
	//std::cout << "\n -------------- gridWay --------------\n";
	grid_way.printGrid();
}
//print &ent according to its type
void GIS_313452542::printEnt(Entity* ent) const {
	if (ent->getType() == Entity::EntityType::Junction) {
		dynamic_cast<Junction*>(ent)->print();
	}
	else if (ent->getType() == Entity::EntityType::Way) {
		dynamic_cast<Way*>(ent)->print();
	}
	else if (ent->getType() == Entity::EntityType::POI) {
		dynamic_cast<POI*>(ent)->print();
	}
}
//prints the idEntityMap: {id: entity}
void GIS_313452542::print() const {
	//std::cout << "\n -------------- EntityIDMap --------------\n";
	printEntityIDMap();
	//std::cout << "\n -------------- NameIdVecMap --------------\n";
	printNameIdVecMap();


	printGrid();
}

//---------------------getEntityClosestPoint------------

Coordinates GIS_313452542::getEntityClosestPoint(const std::shared_ptr<Entity> ent, const Coordinates coordinates)const {
	if (ent->getType() == Entity::EntityType::Junction) {
		return dynamic_cast<Junction*>(ent.get())->getCoordinates();
	}
	else if (ent->getType() == Entity::EntityType::POI) {
		POI poi = (*dynamic_cast<POI*>(ent.get()));
		return getPOIClosestPoint(poi, coordinates);
	}
	else {
		Way way = (*dynamic_cast<Way*>(ent.get()));
		return getWayClosestPoint(way, coordinates).first;
	}
}

Coordinates GIS_313452542::getPOIClosestPoint(const POI& poi, const Coordinates& coordinates)const {

	const Circle& circle = poi.getShape();
	if (ispointInCircle(circle, coordinates)) { return coordinates; }

	Coordinates closest_poi_coor = CoordinatesMath::closestPointOnCircle(
		coordinates, circle.getCoordinates(), circle.getRadius());

	return closest_poi_coor;
}
bool GIS_313452542::ispointInCircle(const Circle& circle, const Coordinates& point) const {
	return (CoordinatesMath::calculateDistance(circle.getCoordinates(), point) <= circle.getRadius());
}

std::pair<Coordinates, std::size_t> GIS_313452542::getWayClosestPoint(const Way& way, const Coordinates& coordinats) const {

	std::vector<Coordinates> curves = way.getCurves();
	Coordinates to = idEntityMap.findJunction(way.getToEntityId())->second.getCoordinates();
	Coordinates from = idEntityMap.findJunction(way.getFromEntityId())->second.getCoordinates();
	std::pair<Coordinates, std::size_t> closest_pair = { from,0 };
	Meters min_dist = Meters(0);
	if (curves.size() > 0) {
		auto coor_dist = CoordinatesMath::closestPointOnSegmentAndDistance(coordinats, from, curves.at(0));
		min_dist = coor_dist.second;
		closest_pair = { coor_dist.first,0 };
		coor_dist = CoordinatesMath::closestPointOnSegmentAndDistance(coordinats, curves.at(curves.size() - 1), to);
		if (coor_dist.second < min_dist) {
			min_dist = coor_dist.second;
			closest_pair = { coor_dist.first,curves.size() };
		}
		for (std::size_t i = 0; i < curves.size() - 1; ++i) {
			coor_dist = CoordinatesMath::closestPointOnSegmentAndDistance(coordinats, curves[i], curves[i + 1]);
			if (coor_dist.second < min_dist) {
				min_dist = coor_dist.second;
				closest_pair = { coor_dist.first,i + 1 };
			}
		}
		return closest_pair;
	}
	else {//curves size = 0
		closest_pair = { CoordinatesMath::closestPointOnSegment(coordinats, from, to), 0 };
		return closest_pair;
	}
}

std::vector<EntityId> GIS_313452542::getWaysByJunction(const EntityId& junctionId) const {
	auto itr = juncId_waysId_map.find(junctionId);
	if (itr != juncId_waysId_map.end()) {
		return std::vector<EntityId>(itr->second.begin(), itr->second.end());
	}
	else {
		return std::vector<EntityId>();
	}
}

const AbstractWay& GIS_313452542::getWay(const EntityId& wayId) const {
	auto itr = idEntityMap.findWay(wayId);
	if (itr != idEntityMap.endWay()) {		
		return itr->second;
	}
	else {
		throw std::invalid_argument("Exeption at getWay: wayId is not an EntityId of a valid way");
	}
}



std::optional<Coordinates> GIS_313452542::getEntityClosestPoint(const EntityId& id,
	const Coordinates& coordinates) const {

	auto p_ent = idEntityMap.findEntity(id);
	if (p_ent != nullptr) {
		return { getEntityClosestPoint(p_ent, coordinates) };
	}
	else return {};
}