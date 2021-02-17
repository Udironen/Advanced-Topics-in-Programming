#include "Simulator.h"
const std::string Simulator::STRANGE_LOG = "strange_GIS_results.log";

std::vector<RequestNode>			Simulator::requests;
size_t								Simulator::num_threads;
std::mutex							Simulator::my_mutex;
std::mutex							Simulator::work_mutex;
std::condition_variable				Simulator::my_cv;
std::condition_variable				Simulator::work_cv;
bool								Simulator::not_empty;
bool								Simulator::done;
std::vector<Simulator::ThreadNode>	Simulator::thread_nodes;
std::atomic<int>					Simulator::work_counter;


void Simulator::simulate(const std::filesystem::path& output) {
	std::vector <void*> gis_handels{};
	std::vector <void*> navigation_handels{};
	bool  not_first_loop = false;
	std::vector<std::thread> threads;

	initRequests();
	for (const auto& gis_name : gis_names) {
		done = false;
		work_counter.store(navigation_names.size() * requests.size());
		for (size_t th = 0; th < Simulator::num_threads; ++th) {
			threads.push_back(std::thread(workerThread));
		}		
		void* gis_handle = dlopen((gis_absulute_path / (gis_name + ".so")).c_str(), RTLD_LAZY);

		if (!gis_handle) {
			continue;
		}
		gis_handels.push_back(gis_handle);
		size_t reg_gis_size = Registrar::getInstance().sizeGISs();
		auto gis = std::move(Registrar::getInstance().getGIS_s()[reg_gis_size - 1]);
		gis->loadMapFile(map_file);
		auto validator = std::make_shared<NavigationValidator>(NavigationValidator(*gis));
		int index = -1;
		for (size_t i_nav = 0; i_nav < navigation_names.size(); ++i_nav) {
			void* navigation_handle = dlopen((navigation_absulute_path / (navigation_names[i_nav] + ".so")).c_str(), RTLD_LAZY);
			if (!navigation_handle) {
				continue;
			}
			std::shared_ptr<AbstractNavigation> navigation;

			navigation_handels.push_back(navigation_handle);
			size_t reg_navigation_size = Registrar::getInstance().sizeNavigations();
			if (not_first_loop) {
				index++;
			}

			for (size_t req = 0; req < requests.size(); req++) {

				//NavigationGIS nav_gis = NavigationGIS(*gis);
				auto nav_gis_ptr = std::make_shared<NavigationGIS>(NavigationGIS(*gis));
				if (not_first_loop) {
					navigation = Registrar::getInstance().getNavigations(*nav_gis_ptr)[reg_navigation_size + index - navigation_names.size()];

				}

				else {
					navigation = Registrar::getInstance().getNavigations(*nav_gis_ptr)[reg_navigation_size - 1];
				}
				Simulator::ThreadNode&& thread_node = Simulator::ThreadNode(navigation,
					validator,
					nav_gis_ptr,
					gis_name,
					requests[req].getStart(),
					requests[req].getEnd(),
					i_nav,
					req);

				{
					std::lock_guard<std::mutex> my_lock(my_mutex);
					thread_nodes.push_back(std::move(thread_node));
					not_empty = true;
				}
				my_cv.notify_one();
			}
		}		

		{
			std::unique_lock<std::mutex> work_lock(work_mutex);
			work_cv.wait(work_lock, [] {return done; });
		}
		{
			std::unique_lock<std::mutex> work_lock(work_mutex);
			not_empty = true;
			my_cv.notify_all();
		}


		for (auto& thread : threads) {
			if (thread.joinable()) {
				thread.join();
			}
		}
		not_empty = false;
		not_first_loop = true;
		index = -1;
	}

	checkConsensusForAll();
	computeScoring();
	loadScoresToFile(output);
	printToStrange();
	closeAllHandels(gis_handels, navigation_handels);
}

//

void Simulator::closeAllHandels(std::vector<void*>& gis_handels, std::vector<void*>& navigation_handels) {
	Registrar::getInstance().clear();
	requests.clear();

	for (auto& navigation_handel : navigation_handels) {
		dlclose(navigation_handel);

	}
	for (auto& gis_handle : gis_handels) {
		dlclose(gis_handle);

	}

}

void Simulator::initRequests() {
	for (auto& request : requests) {
		for (const auto& name : navigation_names) {
			request.addNavigationNode(std::move(NavigationNode(name)));
		}
	}
}
void Simulator::checkConsensusForAll() {
	for (auto& request : requests) {

		for (auto& navi_node : request.getNavigationNodes()) {
			if (navi_node.getIsValidDistance()) {

				checkConsensusDistance(request, navi_node);

			}
			if (navi_node.getIsValidTime()) {

				checkConsensusTime(request, navi_node);

			}
		}
	}
}
bool Simulator::checkConsensusDistance(RequestNode& request, NavigationNode& navi_node)
{
	auto groups = Groups<Meters>(gis_names.size());


	divideToGroupsDistance(navi_node, groups);


	if (!groups.hasConsensus()) {

		request.setValid(false);
		return false;
	}

	updateNavigationNode(navi_node, groups);

	request.updateBestRouteDistance(navi_node);
	return true;
}
bool Simulator::checkConsensusTime(RequestNode& request, NavigationNode& navi_node)
{
	auto groups = Groups<Minutes>(gis_names.size());

	divideToGroupsTime(navi_node, groups);
	if (!groups.hasConsensus()) {
		request.setValid(false);
		return false;
	}
	updateNavigationNode(navi_node, groups);
	request.updateBestRouteTime(navi_node);

	return true;
}

void Simulator::updateNavigationNode(NavigationNode& navi_node, const Groups<Meters>& groups)
{
	for (const auto& name : groups.getBiggestGroup().getnames()) {
		navi_node.updateValidGISConsensusDistance(name);
		navi_node.updateRouteDistance(name);
	}
}
void Simulator::updateNavigationNode(NavigationNode& navi_node, const Groups<Minutes>& groups)
{
	for (const auto& name : groups.getBiggestGroup().getnames()) {
		navi_node.updateValidGISConsensusTime(name);
		navi_node.updateRouteTime(name);
	}
}

void Simulator::divideToGroupsDistance(const NavigationNode& navi_node, Groups<Meters>& groups)
{

	for (const auto& gis_node : navi_node.getGISNodes()) {
		Meters meters = gis_node.second.getDistanceRoute().totalLength();
		groups.insert(meters, gis_node.second.getGISName());
	}
}
void Simulator::divideToGroupsTime(const NavigationNode& navi_node, Groups<Minutes>& groups)
{
	for (const auto& gis_node : navi_node.getGISNodes()) {
		Minutes minutes = gis_node.second.getTimeRoute().estimatedDuration();
		groups.insert(minutes, gis_node.second.getGISName());
	}
}
void Simulator::computeScoring() {
	for (auto& request : requests) {
		if (request.getIsValid()) {
			for (auto& navi_node : request.getNavigationNodes()) {
				bool best = false;				
				if (!navi_node.getIsValidDistance()) {
					std::wcout << __LINE__ << "\n";
			
					navi_node.addToScore(minus_one);
				}
				else {				
					if ((NavigationNode::compareRoutesByDistance(
						navi_node.getDistanceRoute(),
						request.getBestRouteDistance())) == 0)
					{				
						navi_node.addToScore(plus_one);					
						if (navi_node.getBestGISCounterDistance() ==
							request.getBestGISCounterDistance()) {						
							best = true;
						}
					}
				}
				if (!navi_node.getIsValidTime()) {			
					navi_node.addToScore(minus_one);
					
				}
				else {					
					if (NavigationNode::compareRoutesByTime(
						navi_node.getTimeRoute(),
						request.getBestRouteTime()) == 0)
					{						
						navi_node.addToScore(plus_one);						
						if (navi_node.getBestGISCounterTime() ==
							request.getBestGISCounterTime()) {
							best = true;
						}

					}
				}								
				if (best) {					
					navi_node.addToScore(plus_one);					
				}				
			}
		}
	}
}


void Simulator::loadScoresToFile(const std::filesystem::path& output) const {
	std::ofstream results(output);
	printHeadline(results);
	for (std::size_t i = 0; i < navigation_names.size(); ++i) {
		int total_score = 0;
		results << navigation_names[i] << ",";
		for (const auto& request : requests) {
			if (request.getIsValid()) {
				int score = request.getNavigationNodes()[i].getScore();
				total_score += score;
				results << score << ",";
			}
		}
		results << total_score << "\n";
	}
	results.close();
}
void Simulator::printRequestToFile(const RequestNode& request, std::ofstream& file) const {
	file << "{" << (double)request.getStart().longitude() << """,""" << " ";
	file << (double)request.getStart().latitude() << "}-";
	file << "{" << (double)request.getEnd().longitude() << """,""" << " ";
	file << (double)request.getEnd().latitude() << "},";
}
void Simulator::printHeadline(std::ofstream& results) const {
	results << "Navigation Algorithm,";
	for (const auto& request : requests) {
		if (request.getIsValid()) {
			printRequestToFile(request, results);
		}
	}
	results << "Total Score" << std::endl;
}
void Simulator::printToStrange() const {
	std::ofstream log_file;
	log_file.open(STRANGE_LOG, std::ofstream::in |
		std::ofstream::out | std::ofstream::app);
	for (const auto& request : requests) {
		for (const auto& navi_node : request.getNavigationNodes()) {
			for (const auto& gis_node : navi_node.getGISNodes()) {
				if (!gis_node.second.getInConsensusDistance()) {
					printError(log_file,
						navi_node.getNavigationName(),
						"shortest_distance",
						gis_node.second.getGISName(),
						request,
						gis_node.second.getDistanceRoute().totalLength(),
						gis_node.second.getDistanceRoute().estimatedDuration(),
						gis_node.second.getIsValidDistance());
				}
				if (!gis_node.second.getInConsensusTime()) {
					printError(log_file,
						navi_node.getNavigationName(),
						"shortest_time",
						gis_node.second.getGISName(),
						request,
						gis_node.second.getTimeRoute().totalLength(),
						gis_node.second.getTimeRoute().estimatedDuration(),
						gis_node.second.getIsValidTime());
				}
			}
		}
	}
	log_file.close();
}
void Simulator::printError(std::ofstream& file,
	const	std::string& navi_name,
	const	std::string& route_type,
	const	std::string& gis_name,
	const	RequestNode& request,
	const	Meters			distance,
	const	Minutes			time,
	const	bool			valid) const
{
	file << navi_name << ", ";
	file << route_type << ", ";
	file << gis_name << ", ";
	printRequestToFile(request, file);
	file << (double)distance << ", ";
	file << (double)time << ", ";
	file << (int)valid << "\n ";
}