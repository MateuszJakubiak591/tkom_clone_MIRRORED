#include "diagnostics/ErrorHandler.hpp"

#include <iomanip>
#include <sstream>
#include <utility>

ErrorHandler::ErrorHandler(std::string sourceCode)
   : sourceCode_(std::move(sourceCode)) {}

void ErrorHandler::report(
   ErrorType type,
   const std::string& message,
   const SourceLocation& location
) {
   errors_.push_back(Error{type, message, location});
}

bool ErrorHandler::hasErrors() const {
   return !errors_.empty();
}

std::size_t ErrorHandler::errorCount() const {
   return errors_.size();
}

const std::vector<Error>& ErrorHandler::errors() const {
   return errors_;
}

void ErrorHandler::printErrors(std::ostream& out) const {
   for (const Error& error : errors_) {
      out << error.toString() << "\n";

      printSourceSnippet(out, error.location);

      out << "\n";
   }
}

void ErrorHandler::printLastWarning(std::ostream& out) const {
   if (errors_.empty()) {
      return;
   }

   const Error& error = errors_.back();
   out << error.toString() << "\n";
   printSourceSnippet(out, error.location);
   out << "\n";
}

std::string ErrorHandler::getLine(int lineNumber) const {
   if (lineNumber <= 0) {
      return "";
   }

   std::istringstream stream(sourceCode_);
   std::string line;

   int currentLine = 1;

   while (std::getline(stream, line)) {
      if (currentLine == lineNumber) {
         return line;
      }

      currentLine++;
   }

   return "";
}

void ErrorHandler::printSourceSnippet(
   std::ostream& out,
   const SourceLocation& location
) const {
   if (sourceCode_.empty()) {
      return;
   }

   std::string line = getLine(location.line);

   if (line.empty()) {
      return;
   }

   out << line << "\n";

   int column = location.column;

   if (column < 1) {
      column = 1;
   }

   for (int i = 1; i < column; ++i) {
      out << ' ';
   }

   out << "^\n";
}
