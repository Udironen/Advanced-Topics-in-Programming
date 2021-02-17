#pragma once 

#include "GISNamedTypes.h"
#include "Restrictions.h"
#include "AbstractWay.h"
#include <vector>
#include <tuple>

class AbstractGIS {
public:
  /**
   * @brief enum to handle std::tuple<Coordinates, EntityId, std::size_t> from getWayClosestPoint
   * 
   */
  enum ClosestPoint {Coord, WayId, SegmentIndex};

  virtual ~AbstractGIS() {}
  /**
   * @brief 
   * 
   * @param filename - the map file to be loaded
   * @return std::vector<EntityId> - by order of definition in map file 
   */
  virtual std::vector<EntityId> loadMapFile(const std::string& filename) = 0;
  
  /**
   * @brief Get the Ways By Junction object
   * 
   * @param junction_id
   * @return std::vector<EntityId> 
   */
  virtual std::vector<EntityId> getWaysByJunction(const EntityId& junction_id) const = 0;

  /**
   * @brief Get the Way Closest Point object
   * 
   * @param coords 
   * @return std::tuple<Coordinates, EntityId, std::size_t> 
   */
  virtual std::tuple<Coordinates, EntityId, std::size_t> getWayClosestPoint(const Coordinates& coords) const = 0;
  
  /**
   * @brief Get the Way Closest Point object
   * 
   * @param coords 
   * @param res 
   * @return std::tuple<Coordinates, EntityId, std::size_t> 
   */
  virtual std::tuple<Coordinates, EntityId, std::size_t> getWayClosestPoint(const Coordinates& coords, const Restrictions& res) const = 0;
  
  /**
   * @brief Get the Way object
   * 
   * @param way_id 
   * @return const AbstractWay& 
   */
  virtual const AbstractWay& getWay(const EntityId& way_id) const = 0;
};

