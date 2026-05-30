#include "diagnostics/Error.hpp"

std::string errorTypeToString(ErrorType type) {
   switch (type) {
      case ErrorType::Lexical:
         return "Lexical";

      case ErrorType::Parser:
         return "Parser";

      case ErrorType::Runtime:
         return "Runtime";

      case ErrorType::Semantic:
         return "Semantic";
   }

   return "Unknown";
   // w przyszłości będą też błędy semantyczne, ale w sumie zbędne
   // to return Unkown
}

std::string Error::toString() const {
   std::ostringstream oss;

   oss << "Error [" << errorTypeToString(type) << "] "
       << "at " << location.filename
       << ":" << location.line
       << ":" << location.column
       << ": " << message;

   return oss.str();
}
