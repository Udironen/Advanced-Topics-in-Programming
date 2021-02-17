#pragma once 

#include "AbstractGIS.h"
#include <memory> 
#include <functional> 

class GISRegistration {
public:
	GISRegistration(std::function<std::unique_ptr<AbstractGIS>()>);
};

#ifndef EX1_EX2
#define REGISTER_GIS(class_name) \
GISRegistration register_me_##class_name \
	([]{return std::make_unique<class_name>();} );
#else 
#define REGISTER_GIS(class_name) bool GIS_TESTING{true};
#endif