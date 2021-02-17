#include "Simulator.h"
#include <array>
#include <string>
#include <vector>
#include<stdio.h> 

#define GIS  "GIS"
#define NAVIGATION "Navigation"


bool isDouble(const char* str)
{
	char* endptr = 0;
	strtod(str, &endptr);

	if (*endptr != '\0' || endptr == str)
		return false;
	return true;
}



bool isInt(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) {
		++it;
	}
	return !s.empty() && it == s.end();
}
const Coordinates getCoordinates(std::string coordinates) {
	size_t pos = coordinates.find(",");

	std::string longi_str = coordinates.substr(1, pos - 1);
	std::string lati_str = coordinates.substr(pos + 1);

	double longi = std::stod(longi_str);
	double lati = std::stod(lati_str);
	return Coordinates(Longitude(longi), Latitude(lati));
}
bool validCoordinates(const std::string& coords) {

	if (coords.at(0) != '{') {
		return false;

	}
	if (coords.at(coords.length() - 1) != '}') {
		return false;

	}
	size_t pos = coords.find(",");
	if (pos == std::string::npos) {
		return false;
	}
	std::string longi = coords.substr(1, pos - 1);


	std::string lati = coords.substr(pos + 1, coords.length() - pos - 2);
	if (!isDouble(longi.c_str())) {
		return false;
	}
	if (!isDouble(lati.c_str())) {
		return false;

	}
	return true;
}
bool validRangeCoodinates(const Coordinates& coord) {
	if (((double)coord.longitude() > 180) || ((double)coord.longitude() < -180)) return false;
	if (((double)coord.latitude() > 90) || ((double)coord.latitude() < -90)) return false;
	return true;
}
void init_request(std::vector<RequestNode>& requests, const std::string& line) {
	size_t pos = line.find("}-");
	if (pos == std::string::npos) {
		std::cout << "invalid: " << line << "\n";
		return;
	}

	std::string start_str = line.substr(0, pos + 1);
	std::string end_str = line.substr(pos + 2);
	if ((!validCoordinates(start_str)) || (!validCoordinates(end_str))) {
		std::cout << "invalid: " << line << "\n";
		return;
	}

	Coordinates start = getCoordinates(start_str);
	Coordinates end = getCoordinates(end_str);
	if (!(validRangeCoodinates(start)) || (!validRangeCoodinates(end))) {
		std::cout << "invalid: " << line << "\n";
	}
	else {
		requests.push_back(RequestNode(std::move(start), std::move(end)));
	}
}


void getRequestNodes(const std::string& path, std::vector<RequestNode>& requests) {
	std::ifstream file(path);
	std::string line;
	if (file.is_open()) {
		while (std::getline(file, line)) {
			init_request(requests, line);
		}
		file.close();
	}
}



void getnames(const std::string& path, std::vector<std::string>& names, const char* type) {
	size_t posi = path.length() + 1;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		std::string file_name = std::string(entry.path()).substr(posi);

		std::string type_token = file_name.substr(0, file_name.find("_"));
		size_t found = file_name.find(".");
		if (found == std::string::npos) {
			continue;
		}
		std::string so_token = file_name.substr(found + 1);


		if ((type_token.compare(type) == 0) && (so_token.compare((std::string)"so") == 0)) {


			names.push_back(file_name.substr(0, file_name.length() - 3));
		}
	}

}

bool initInputs(std::array<std::string, 6>& inputs, char** argv, int len) {
	for (int i = 1; i < len; i += 2) {
		if (strcmp(argv[i], "-num_threads") == 0) {
			if (!(isInt(argv[i + 1]))) {
				std::cout << "invalid -num_threads argument" << std::endl;
				return false;
			}
			inputs[0] = argv[i + 1];
		}
		else if (strcmp(argv[i], "-navigation") == 0) {
			if (!(std::filesystem::is_directory(argv[i + 1]))) {
				std::cout << "invalid -navigation argument" << std::endl;
				return false;
			}
			inputs[1] = argv[i + 1];
		}
		else if (strcmp(argv[i], "-gis") == 0) {
			if (!(std::filesystem::is_directory(argv[i + 1]))) {
				std::cout << "invalid -gis argument" << std::endl;
				return false;
			}
			inputs[2] = argv[i + 1];
		}
		else if (strcmp(argv[i], "-map_file") == 0) {
			if (!(std::filesystem::is_regular_file(argv[i + 1]))) {
				std::cout << "invalid -map_file argument" << std::endl;
				return false;
			}
			inputs[3] = argv[i + 1];
		}
		else if (strcmp(argv[i], "-navigation_requests") == 0) {
			if (!(std::filesystem::is_regular_file(argv[i + 1]))) {
				std::cout << "invalid -navigation_requests argument" << std::endl;
				return false;
			}
			inputs[4] = argv[i + 1];
		}
		else if (strcmp(argv[i], "-output") == 0) {
			if (!(std::filesystem::is_directory(argv[i + 1]))) {
				std::cout << "invalid -output argument" << std::endl;
				return false;
			}
			inputs[5] = argv[i + 1];
			inputs[5] += std::string("simulation.results");
		}
		else {
			std::cout << "invalid input : " << argv[i] << std::endl;
			return false;
		}
	}
	return true;
}
int main(int argc, char** argv) {

	//inputs[0] = num_threads
	//inputs[1] = navigation_path
	//inputs[2] = gis_path
	//inputs[3] = map_file
	//inputs[4] = navigation_requests
	//inputs[5] = output

	std::array<std::string, 6> inputs;
	inputs[0] = "1";
	inputs[5] = "./simulation.results";
	if (!initInputs(inputs, argv, argc)) {
		return 0;
	}
	
	std::vector<std::string> gis_names{};
	std::vector<std::string> navigations_names{};
	std::vector<RequestNode> requests{};
	getnames(inputs[1], navigations_names, NAVIGATION);
	getnames(inputs[2], gis_names, GIS);
	getRequestNodes(inputs[4], requests);

	size_t num_threads = 1;
	int num = std::stoi(inputs[0]);
	int max_threads = navigations_names.size() * requests.size();


	if (num > 1) {
		if (num > max_threads) {
			num_threads = (size_t)max_threads;
		}
		else {
			num_threads = (size_t)num - 1;
		}
	}	

	Simulator simulator = Simulator(std::filesystem::path{ inputs[1] },
		std::filesystem::path{ inputs[2] },
		navigations_names,
		gis_names,
		inputs[3],
		requests,
		num_threads);

	simulator.simulate(inputs[5]);

}
