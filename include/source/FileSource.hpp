#pragma once

#include <fstream>
#include <string>

#include "source/CharSource.hpp"

class FileSource : public CharSource {
public:
   explicit FileSource(const std::string& filename);

   char peek() override;
   char peekNext() override;
   char advance() override;

   bool isAtEnd() const override;
   SourceLocation currentLocation() const override;

private:
   void fillLookahead();

   std::string filename_;
   std::ifstream file_;

   char current_ = '\0';
   char next_ = '\0';

   bool currentValid_ = false;
   bool nextValid_ = false;

   int line_ = 1;
   int column_ = 1;
};