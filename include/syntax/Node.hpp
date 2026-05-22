#pragma once

#include "source/SourceLocation.hpp"

class Node {
public:
   explicit Node(SourceLocation location)
      : location_(std::move(location)) {}

   virtual ~Node() = default;

   const SourceLocation& location() const {
      return location_;
   }

private:
   SourceLocation location_;
};