#pragma once

#include "source/SourceLocation.hpp"

class Visitor;

class Node {
public:
   explicit Node(SourceLocation location)
      : location_(std::move(location)) {}

   virtual ~Node() = default;

   virtual void accept(Visitor& visitor) const;

   const SourceLocation& location() const {
      return location_;
   }

private:
   SourceLocation location_;
};
