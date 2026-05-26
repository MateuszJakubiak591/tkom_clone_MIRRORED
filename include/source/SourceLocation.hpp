#pragma once

#include <string>

struct SourceLocation {
   std::string filename;
   int line = 1;
   int column = 1;
};

inline bool operator==(const SourceLocation& lhs, const SourceLocation& rhs) {
   return lhs.filename == rhs.filename
       && lhs.line == rhs.line
       && lhs.column == rhs.column;
}