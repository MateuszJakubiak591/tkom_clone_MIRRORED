#pragma once

#include "source/CharSource.hpp"
#include "lexer/Token.hpp"

class Lexer {
public:
   explicit Lexer(CharSource& source);

   Token nextToken();

private:
   CharSource& source_;

   int parenDepth_ = 0;
   int bracketDepth_ = 0;
};
