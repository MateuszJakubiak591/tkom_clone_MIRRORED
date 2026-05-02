#pragma once

#include <string>

#include "source/CharSource.hpp"

class StringSource : public CharSource {
public:
   StringSource(std::string filename, std::string content);

   char peek() override;
   char peekNext() override;

   char advance() override;

   bool isAtEnd() const override;
   SourceLocation currentLocation() const override;

private:
   std::string filename_;
   std::string content_;
   std::size_t pos_ = 0;
   int line_ = 1;
   int column_ = 1;
};