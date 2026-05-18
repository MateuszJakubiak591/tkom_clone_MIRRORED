#pragma once

#include <string>
#include <sstream>

#include "source/SourceLocation.hpp"

enum class ErrorType {
   Lexical,
   Parser
};

std::string errorTypeToString(ErrorType type);

struct Error {
   ErrorType type;
   std::string message;
   SourceLocation location;

   Error(
      ErrorType type,
      std::string message,
      SourceLocation location
   )
      : type(type),
        message(std::move(message)),
        location(std::move(location)) {}
   
   std::string toString() const;
};