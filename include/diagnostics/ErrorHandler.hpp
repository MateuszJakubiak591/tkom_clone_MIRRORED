#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "diagnostics/Error.hpp"

class ErrorHandler {
public:
   ErrorHandler() = default;

   explicit ErrorHandler(std::string sourceCode);

   void report(
      ErrorType type,
      const std::string& message,
      const SourceLocation& location
   );

   bool hasErrors() const;
   std::size_t errorCount() const;

   const std::vector<Error>& errors() const;

   void printErrors(std::ostream& out) const;

private:
   std::vector<Error> errors_;
   std::string sourceCode_;

   std::string getLine(int lineNumber) const;
   void printSourceSnippet(std::ostream& out, const SourceLocation& location) const;
};