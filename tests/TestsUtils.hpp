#pragma once

#include "lexer/Token.hpp"

#include <string>
#include <vector>

void runTest(const std::string& name, void (*testFunction)());

std::vector<Token> tokenizeString(
   const std::string& filename,
   const std::string& code
);

std::vector<Token> tokenizeFile(const std::string& filename);

void assertToken(
   const Token& token,
   TokenType expectedType,
   const std::string& expectedLexeme,
   int expectedLine,
   int expectedColumn
);

void assertNoInvalidTokens(const std::vector<Token>& tokens);