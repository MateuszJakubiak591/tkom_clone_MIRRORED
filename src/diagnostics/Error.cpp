#include "diagnostics/Error.hpp"

std::string errorTypeToString(ErrorType type) {
   switch (type) {
      case ErrorType::Lexical:
         return "Lexical";

      case ErrorType::Parser:
         return "Parser";
   }

   return "Unknown";
   // w przyszłości będą też błędy semantyczne, ale w sumie zbędne
   // to return Unkown
}