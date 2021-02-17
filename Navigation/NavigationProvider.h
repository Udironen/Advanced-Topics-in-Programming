#pragma once

#include "Navigation.h"

class NavigationProvider {
public:
        static auto getNavigation(const NavigationGIS& navigation_gis) {
                return Navigation_313452542{navigation_gis};
        }
};
