#include "../Common/GISRegistration.h"
#include "Registrar.h"

GISRegistration::GISRegistration(std::function<std::unique_ptr<AbstractGIS>()> factory) {
	Registrar::getInstance().registerGIS(factory);
}
