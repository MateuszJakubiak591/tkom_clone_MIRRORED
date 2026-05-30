#pragma once

#include <string>
#include <sstream>

#include "source/SourceLocation.hpp"

enum class ErrorType {
   Lexical,
   Parser,
   Runtime,
   Semantic
};

std::string errorTypeToString(ErrorType type);

struct Error {
   ErrorType type;
   std::string message;
   SourceLocation location;

   std::string toString() const;
};
