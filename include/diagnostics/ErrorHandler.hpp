#pragma once

/// @file ErrorHandler.hpp
/// Diagnostic collection and rendering interfaces.

#include <ostream>
#include <string>
#include <vector>

#include "diagnostics/Error.hpp"

/// Collects diagnostics and renders them together with optional source snippets.
class ErrorHandler {
public:
   ErrorHandler() = default;

   explicit ErrorHandler(std::string sourceCode);

   /// Stores a diagnostic without deciding whether execution should stop.
   virtual void report(
      ErrorType type,
      const std::string& message,
      const SourceLocation& location
   );

   bool hasErrors() const;
   std::size_t errorCount() const;

   const std::vector<Error>& errors() const;

   /// Prints all diagnostics accumulated so far.
   void printErrors(std::ostream& out) const;
   /// Prints only the newest diagnostic, used for live interpreter feedback.
   virtual void printLastWarning(std::ostream& out) const;

private:
   std::vector<Error> errors_;
   std::string sourceCode_;

   std::string getLine(int lineNumber) const;
   void printSourceSnippet(std::ostream& out, const SourceLocation& location) const;
};


/// Diagnostic sink used when the caller intentionally provides no handler.
class NullErrorHandler : public ErrorHandler {
public:
   void report(ErrorType, const std::string&, const SourceLocation&) override {}
   void printLastWarning(std::ostream&) const override {}
};
