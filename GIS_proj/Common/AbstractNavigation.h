#pragma once

#include "GISNamedTypes.h"
#include "NavigationGIS.h"
#include "AbstractRoutes.h"
#include <memory> 

class AbstractNavigation {
protected:
  const NavigationGIS& gis;

public:
  /**
   * @brief Construct a new Abstract Navigation object
   * 
   * @param navigation_gis - A NavigationGIS referencing a GIS object
   */
  AbstractNavigation(const NavigationGIS& navigation_gis) : gis(navigation_gis) {}

  /**
   * @brief Destroy the Abstract Navigation object
   * 
   */
  virtual ~AbstractNavigation() {}
  /**
   * @brief Get the Routes object
   * 
   * @param start 
   * @param end 
   * @return std::unique_ptr<AbstractRoutes> 
   */
  virtual std::unique_ptr<AbstractRoutes> getRoutes(const Coordinates& start, const Coordinates& end) const = 0;
};

