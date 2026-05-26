#include "source/FileSource.hpp"

#include <stdexcept>

FileSource::FileSource(const std::string& filename)
   : filename_(filename),
     file_(filename, std::ios::binary) {
   if (!file_) {
      throw std::runtime_error("Nie można otworzyć pliku: " + filename);
   }

   fillLookahead();
   fillLookahead();
}

void FileSource::fillLookahead() {
   if (!currentValid_) {
      int c = file_.get();

      if (c == EOF) {
         current_ = '\0';
         currentValid_ = false;
      } else {
         current_ = static_cast<char>(c);
         currentValid_ = true;
      }

      return;
   }

   if (!nextValid_) {
      int c = file_.get();

      if (c == EOF) {
         next_ = '\0';
         nextValid_ = false;
      } else {
         next_ = static_cast<char>(c);
         nextValid_ = true;
      }
   }
}

char FileSource::peek() {
   if (!currentValid_) {
      return '\0';
   }

   return current_;
}

char FileSource::peekNext() {
   if (!nextValid_) {
      return '\0';
   }

   return next_;
}

char FileSource::advance() {
   if (!currentValid_) {
      return '\0';
   }

   char c = current_;

   if (c == '\n') {
      line_++;
      column_ = 1;
   } else {
      column_++;
   }

   current_ = next_;
   currentValid_ = nextValid_;

   next_ = '\0';
   nextValid_ = false;

   fillLookahead();

   return c;
}

bool FileSource::isAtEnd() const {
   return !currentValid_;
}

SourceLocation FileSource::currentLocation() const {
   return SourceLocation{filename_, line_, column_};
}