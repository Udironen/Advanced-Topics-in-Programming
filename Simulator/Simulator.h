#pragma once

#include <filesystem>
#include <dlfcn.h>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>
#include "Registrar.h"
#include "NavigationValidator.h"
#include "RequestNode.h"
#include "../UserCommon/Hashers.h"
#include "Groups.h"
#include <list>
#include <array>

#include <thread>
#include <mutex>
#include <condition_variable>

class Simulator {

	struct ThreadNode
	{
		std::shared_ptr<AbstractNavigation>		navigation;
		std::shared_ptr<NavigationValidator>	validator;
		std::shared_ptr<NavigationGIS>			nav_gis;
		std::string								gis_name;
		Coordinates								start;
		Coordinates								end;
		size_t									index_navi;
		size_t									index_req;

		ThreadNode(const	std::shared_ptr<AbstractNavigation>& navigation_,
			const	std::shared_ptr<NavigationValidator>& validator_,
			const	std::shared_ptr<NavigationGIS>& nav_gis_,
			const	std::string& gis_name_,
			const	Coordinates& start_,
			const	Coordinates& end_,
			size_t									index_navi_,
			size_t									index_req_)
			:
			navigation(navigation_),
			validator(validator_),
			nav_gis(nav_gis_),
			gis_name(gis_name_),
			start(start_),
			end(end_),
			index_navi(index_navi_),
			index_req(index_req_) {}

	};

	static const std::string STRANGE_LOG;

	std::filesystem::path navigation_absulute_path;
	std::filesystem::path gis_absulute_path;
	std::vector<std::string> navigation_names;
	std::vector<std::string> gis_names;
	std::filesystem::path map_file;

	static std::vector<RequestNode> requests;




	void initRequests();

	void closeAllHandels(std::vector<void*>& gis_handels, std::vector<void*>& navigation_handels);

	void checkConsensusForAll();

	bool checkConsensusDistance(RequestNode& request, NavigationNode& navi_node);

	bool checkConsensusTime(RequestNode& request, NavigationNode& navi_node);

	void updateNavigationNode(NavigationNode& navi_node, const Groups<Meters>& groups);

	void updateNavigationNode(NavigationNode& navi_node, const Groups<Minutes>& groups);

	void divideToGroupsDistance(const NavigationNode& navi_node, Groups<Meters>& groups);

	void divideToGroupsTime(const NavigationNode& navi_node, Groups<Minutes>& groups);

	void computeScoring();

	void loadScoresToFile(const std::filesystem::path& output) const;



	void printRequestToFile(const RequestNode& request, std::ofstream& file) const;

	void printHeadline(std::ofstream& results) const;
	void printToStrange() const;

	void printError(std::ofstream& file,
		const	std::string& navi_name,
		const	std::string& route_type,
		const	std::string& gis_name,
		const	RequestNode& request,
		const	Meters			distance,
		const	Minutes			time,
		const	bool			valid) const;

	static void workerThread() {

		while (!done)
		{
			std::unique_lock<std::mutex> my_lock(my_mutex);
			while (!not_empty) {
				my_cv.wait(my_lock, [] {return not_empty; });
				if ((not_empty) || (done)) {
					break;
				}
			}

			if (done) {
				my_lock.unlock();
				my_cv.notify_all();
				break;
			}
			size_t length = thread_nodes.size() - 1;
			ThreadNode thread_node = thread_nodes[length];
			thread_nodes.pop_back();
			not_empty = (!thread_nodes.empty());
			work_counter--;
			{
				std::lock_guard<std::mutex> work_lock(work_mutex);
				if (work_counter <= 0) {
					done = true;
					work_cv.notify_all();
				}
			}
			
			my_lock.unlock();
			auto routes = thread_node.navigation->getRoutes(thread_node.start, thread_node.end);

			bool is_valid_distance_route = thread_node.validator->validateRoute(thread_node.start,
				thread_node.end,
				routes->shortestDistance());

			bool is_valid_time_route = thread_node.validator->validateRoute(thread_node.start,
				thread_node.end,
				routes->shortestTime());

			GISNode&& gis_node = GISNode(thread_node.gis_name,
				std::move(routes),
				is_valid_distance_route,
				is_valid_time_route,
				thread_node.nav_gis->getUsageCounter());
			{
				my_lock.lock();
				
				requests[thread_node.index_req].insertGISNode(thread_node.index_navi, std::move(gis_node));
			}
		}
	}


public:
	static int constexpr minus_one = -1;
	static int constexpr plus_one = 1;

	Simulator(const std::filesystem::path& navigation_absulute_path_,
		const std::filesystem::path& gis_absulute_path_,
		const std::vector<std::string>& navigation_names_,
		const std::vector<std::string>& gis_names_,
		const std::filesystem::path& map_file_,
		std::vector<RequestNode>& requests_,
		const size_t						num_threads_)
		:
		navigation_absulute_path(navigation_absulute_path_),
		gis_absulute_path(gis_absulute_path_),
		navigation_names(navigation_names_),
		gis_names(gis_names_),
		map_file(map_file_)
	{

		num_threads = num_threads_;
		requests = std::move(requests_);
		not_empty = false;
		done = false;

	}

	void simulate(const std::filesystem::path& output);

private:
	static size_t num_threads;
	static std::mutex my_mutex;
	static std::mutex work_mutex;
	static std::condition_variable my_cv;
	static std::condition_variable work_cv;
	static bool not_empty;
	static bool done;
	static std::atomic<int> work_counter;
	static std::vector<ThreadNode> thread_nodes;

};