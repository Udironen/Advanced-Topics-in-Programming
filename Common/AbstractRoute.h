#pragma once 

#include "GISNamedTypes.h"
#include <vector>

class AbstractRoute {
public:
  virtual ~AbstractRoute() {}
  virtual const std::vector<std::pair<EntityId, Direction>>& getWays() const = 0;
  virtual const Coordinates& getWayStartPoint() const = 0;
  virtual const Coordinates& getWayEndPoint() const = 0;
  virtual Meters totalLength() const = 0;
  virtual Minutes estimatedDuration() const = 0;
};

