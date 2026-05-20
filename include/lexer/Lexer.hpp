#pragma once

#include <optional>

#include "source/CharSource.hpp"
#include "lexer/Token.hpp"
#include "lexer/TokenSource.hpp"

class Lexer : public TokenSource {
public:
   explicit Lexer(CharSource& source);

   Token nextToken() override;

private:
   CharSource& source_;

   int parenDepth_ = 0;
   int bracketDepth_ = 0;

   std::optional<Token> skipIgnored();

   void skipWhitespace();
   void skipLineComment();

   std::optional<Token> skipBlockComment();
   //std::optional<Token> readNewlineIfNeeded();
   std::optional<Token> readNewline();
   

   std::optional<Token> readSymbolOrOperator(char c, SourceLocation start);

   Token readStringLiteral(SourceLocation start);
   Token readCharLiteral(SourceLocation start);
   Token readNumberLiteral(char first, SourceLocation start);
   Token readIdentifierOrKeyword(char first, SourceLocation start);

   bool readDigitSequenceRest(std::string& text);
};