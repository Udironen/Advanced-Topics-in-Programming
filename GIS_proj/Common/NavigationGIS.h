#pragma once 

#include "AbstractGIS.h"

/**
 * @brief NavigationGIS class 
 *        The class below would be provided by the course staff, see Dependencies comment below
 *        This is a Proxy class for your GIS, the purpose of it is to:
 *        [1] restrict the navigation system to use only certain methods of the GIS class
 *        [2] allow monitoring the calls to GIS by the navigation system via this proxy
 * 
 */
class NavigationGIS {
  const AbstractGIS& gis;
  mutable std::size_t usage_counter;

public:
  /**
   * @brief Construct a new Navigation G I S object
   * 
   * @param gis 
   */
  NavigationGIS(const AbstractGIS& gis): gis(gis), usage_counter(0) {}

  /**
   * @brief Get the Ways By Junction object
   * 
   * @param junctionId 
   * @return std::vector<EntityId> 
   */
  std::vector<EntityId> getWaysByJunction(const EntityId& junctionId) const {
	  ++usage_counter; 
  	return gis.getWaysByJunction(junctionId);
  }

  /**
   * @brief Get the Way Closest Point object
   * 
   * @param coords 
   * @return std::tuple<Coordinates, EntityId, std::size_t> 
   */
  std::tuple<Coordinates, EntityId, std::size_t> getWayClosestPoint(const Coordinates& coords) const {
  	++usage_counter;
	  return gis.getWayClosestPoint(coords);
  }

  /**
   * @brief Get the Way Closest Point object
   * 
   * @param coords 
   * @param res 
   * @return std::tuple<Coordinates, EntityId, std::size_t> 
   */
  std::tuple<Coordinates, EntityId, std::size_t> getWayClosestPoint(const Coordinates& coords, const Restrictions& res) const {
	  ++usage_counter; 
	  return gis.getWayClosestPoint(coords, res);
  }

  /**
   * @brief Get the Way object
   * 
   * @param wayId 
   * @return const Way& 
   */
  const AbstractWay& getWay(const EntityId& wayId) const {
	  ++usage_counter; 
	  return gis.getWay(wayId); 
  }

  /**
   * @brief Get the usage counter
   * 
   * @return std::size_t 
   */
  std::size_t getUsageCounter() const {
  	return usage_counter;
  }
};
