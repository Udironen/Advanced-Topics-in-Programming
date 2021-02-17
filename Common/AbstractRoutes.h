#pragma once 

#include "AbstractRoute.h"

class AbstractRoutes {
public:
  virtual ~AbstractRoutes() {}

  virtual bool isValid() const = 0;

  // following functions can be called only if isValid is true - undefined otherwise
  virtual const AbstractRoute& shortestDistance() const = 0;
  virtual const AbstractRoute& shortestTime() const = 0;

  // following function can be called only if isValid is false - undefined otherwise
  virtual const std::string& getErrorMessage() const = 0;
};

