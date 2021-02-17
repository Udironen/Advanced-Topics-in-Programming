#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include <memory>

#include "../Common/AbstractGIS.h"
#include "../Common/AbstractNavigation.h"
#include "../Common/NavigationRegistration.h"
#include "../Common/GISRegistration.h"

typedef std::vector<std::function<std::shared_ptr<AbstractNavigation>(const NavigationGIS&)>> Navigation_Vec;
typedef std::vector<std::function<std::shared_ptr<AbstractGIS>()>> GIS_Vec;

class Registrar {

	Navigation_Vec				navigation_factories;
	GIS_Vec						gis_factories;
	

	void registerNavigation(std::function<std::unique_ptr<AbstractNavigation>(const NavigationGIS&)> navigationFactory) {
		instance.navigation_factories.push_back(navigationFactory);
	}
	void registerGIS(std::function<std::unique_ptr<AbstractGIS>()> gisFactory) {
		instance.gis_factories.push_back(gisFactory);
	}

public:
	friend class GISRegistration;
	friend class NavigationRegistration;
	//enum {REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_REGISTERED = -2 };
	
	std::vector<std::shared_ptr<AbstractNavigation>> getNavigations(const NavigationGIS& gis ) const {
		std::vector<std::shared_ptr<AbstractNavigation>> navigations;
		for (auto navigationFactoryFunc : navigation_factories) {
			navigations.push_back(navigationFactoryFunc(gis));
		}
		return navigations;
	}
	
	std::vector<std::shared_ptr<AbstractGIS>> getGIS_s() const {
		std::vector<std::shared_ptr<AbstractGIS>> gis_s ;
		for (auto GISFactoryFunc : gis_factories) {
			gis_s.push_back(GISFactoryFunc());
		}
		return gis_s;
	}

	
	void clear(){
		gis_factories.clear();
		navigation_factories.clear();
	}
	void clearNav(){
		navigation_factories.clear();
	}

	size_t sizeNavigations() const {
		return navigation_factories.size();
	}

	size_t sizeGISs() const {
		return gis_factories.size();
	}

	static Registrar& getInstance() {
		return instance;
	}

	private: 
		static Registrar			instance;


};


