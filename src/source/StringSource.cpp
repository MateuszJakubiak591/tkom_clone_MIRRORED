#include "source/StringSource.hpp"

#include <utility>

StringSource::StringSource(std::string filename, std::string content)
   : filename_(std::move(filename)),
     content_(std::move(content)) {}

char StringSource::peek() {
   if (isAtEnd()) {
      return '\0';
   }

   return content_[pos_];
}

char StringSource::peekNext() {
   if (pos_ + 1 >= content_.size()) {
      return '\0';
   }

   return content_[pos_ + 1];
}

char StringSource::advance() {
   if (isAtEnd()) {
      return '\0';
   }

   char c = content_[pos_++];

   if (c == '\n') {
      line_++;
      column_ = 1;
   } else {
      column_++;
   }

   return c;
}

bool StringSource::isAtEnd() const {
   return pos_ >= content_.size();
}

SourceLocation StringSource::currentLocation() const {
   return SourceLocation{filename_, line_, column_};
}