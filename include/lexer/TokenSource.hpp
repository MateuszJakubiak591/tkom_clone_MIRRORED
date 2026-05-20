#pragma once

#include "lexer/Token.hpp"

class TokenSource {
public:
   virtual ~TokenSource() = default;
   virtual Token nextToken() = 0;
};