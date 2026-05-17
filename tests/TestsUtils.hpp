#pragma once

#include "lexer/Token.hpp"

#include <string>
#include <vector>
#include <typeinfo>

#include "parser/Parser.hpp"
#include "diagnostics/ErrorHandler.hpp"


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

void assertToken(
   const Token& token,
   TokenType expectedType,
   const std::string& expectedLexeme,
   const TokenValue& expectedValue = std::monostate{} // Domyślnie brak wartości
);

void assertNoInvalidTokens(const std::vector<Token>& tokens);

StmtPtr parseString(const std::string& code, ErrorHandler* errHandler = nullptr);