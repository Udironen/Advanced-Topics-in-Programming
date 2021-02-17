#pragma once 
////////////////////////////////////////need to change back!
//#include "../Common/GISNamedTypes.h"
#include "GISNamedTypes.h"
class AbstractWay {
public:
  /**
   * @brief Destroy the Abstract Way object
   * 
   */
  virtual ~AbstractWay() {}

  /**
   * @brief Get the Junctions object
   * 
   * @return std::pair<EntityId, EntityId> - as {from junction, to junction}
   */
  virtual std::pair<EntityId, EntityId> getJunctions() const = 0; // from, to
  
  /**
   * @brief Get the Length object
   * 
   * @return Meters - total length of way (total of all way segments, through all curves)
   */
  virtual Meters getLength() const = 0;

  /**
   * @brief 
   * 
   * @return true - is bidirectional
   * @return false - is unidirectional
   */
  virtual bool isBidirectional() const = 0;

  /**
   * @brief Get the Speed Limit object
   * 
   * @return int - as Km/h
   */
  virtual int getSpeedLimit() const = 0;
  
  /**
   * @brief 
   * 
   * @return true - is highway
   * @return false - isn't
   */
  virtual bool isHighway() const = 0;

  /**
   * @brief 
   * 
   * @return true - is toll way
   * @return false - isn't
   */
  virtual bool isToll() const = 0;

  /**
   * @brief Get the Segment Parts On Way object
   * 
   * @param segment 
   * @param c 
   * @return std::pair<Meters, Meters> - as {from-junction->point, point->to-junction}
   */
  virtual std::pair<Meters, Meters> getSegmentPartsOnWay(std::size_t segment, const Coordinates& c) const = 0;

  /**
   * @brief Get the From Junction Coordinates object
   * 
   * @return const Coordinates& 
   */
  virtual const Coordinates& getFromJunctionCoordinates() const = 0;
  
  /**
   * @brief Get the To Junction Coordinates object
   * 
   * @return const Coordinates& 
   */
  virtual const Coordinates& getToJunctionCoordinates() const = 0;

   /**
   * @brief Get the EntityId of Way object
   *
   * @return const EntityId& - Way's id
   */
  virtual const EntityId& getId() const = 0;
};

