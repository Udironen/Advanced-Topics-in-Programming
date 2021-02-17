#include "../Common/NavigationRegistration.h"
#include "Registrar.h"

NavigationRegistration::NavigationRegistration(std::function<std::unique_ptr<AbstractNavigation>(const NavigationGIS&)> factory) {
	Registrar::getInstance().registerNavigation(factory);
}
