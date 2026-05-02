#pragma once

#include "source/SourceLocation.hpp"

class CharSource {
public:
   virtual ~CharSource() = default;

   virtual char peek() = 0;
   virtual char peekNext() = 0;
   virtual char advance() = 0;

   virtual bool isAtEnd() const = 0;
   virtual SourceLocation currentLocation() const = 0;
};