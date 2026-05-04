#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "source/StringSource.hpp"
#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
#include "lexer/CharBasicLib.hpp"

#include <unordered_map>


Lexer::Lexer(CharSource& source)
   : source_(source) {}

Token Lexer::nextToken() {
   // Pomijanie białych znaków, komentarzy
   while (!source_.isAtEnd()) {
      char c = source_.peek();

      if (isWhiteSpace(c)) {
         source_.advance();
         continue;
      }

      // Pomijanie do końca linii dla komentarza //
      if (c == '/' && source_.peekNext() == '/') {
         while (!source_.isAtEnd() && source_.peek() != '\n') {
            source_.advance();
         }
         continue;
      }

      // Pomijanie wewnątrz komentarza wieloliniowego /* */
      if (c == '/' && source_.peekNext() == '*') {
         SourceLocation start = source_.currentLocation();

         // Pominięcie /*
         source_.advance();
         source_.advance();

         bool closed = false;

         while (!source_.isAtEnd()) {
            if (source_.peek() == '*' && source_.peekNext() == '/') {
               source_.advance();
               source_.advance();
               closed = true;
               break;
            }

            source_.advance();
         }

         if (!closed) {
            return makeInvalidToken("unterminated block comment", start);
         }

         continue;
      }


      if (c == '\n') {
         SourceLocation start = source_.currentLocation();
         source_.advance();
         // Znak nowej linii jest ignorowany, gdy jesteśmy
         // wewnątrz wyrażenia lub literału listy
         if (parenDepth_ > 0 || bracketDepth_ > 0) {
            continue;
         }

         return makeToken(TokenType::Newline, "\\n", start);
      }

      break;
   }

   SourceLocation start = source_.currentLocation();

   if (source_.isAtEnd()) {
      return makeToken(TokenType::EndOfFile, "", start);
   }

   char c = source_.advance();

   switch (c) {
      case '(':
         parenDepth_++;
         return makeToken(TokenType::LParen, "(", start);

      case ')':
         if (parenDepth_ > 0) {
            parenDepth_--;
         }
         return makeToken(TokenType::RParen, ")", start);

      case '[':
         bracketDepth_++;
         return makeToken(TokenType::LBracket, "[", start);

      case ']':
         if (bracketDepth_ > 0) {
            bracketDepth_--;
         }
         return makeToken(TokenType::RBracket, "]", start);

      case '{': return makeToken(TokenType::LBrace, "{", start);
      case '}': return makeToken(TokenType::RBrace, "}", start);
      case ',': return makeToken(TokenType::Comma, ",", start);
      case '.': return makeToken(TokenType::Dot, ".", start);
      case ':': return makeToken(TokenType::Colon, ":", start);
      case '?': return makeToken(TokenType::OpFilter, "?", start);
      case '%': return makeToken(TokenType::OpGroup, "%", start);
      case '+': return makeToken(TokenType::Plus, "+", start);
      case '*': return makeToken(TokenType::Multiply, "*", start);
      case '/': return makeToken(TokenType::Divide, "/", start);
      case '^': return makeToken(TokenType::Power, "^", start);

      case '-':
         if (source_.peek() == '>') {
            source_.advance();
            return makeToken(TokenType::Arrow, "->", start);
         }
         return makeToken(TokenType::Minus, "-", start);

      case '!':
         if (source_.peek() == '=') {
            source_.advance();
            return makeToken(TokenType::NotEqual, "!=", start);
         }
         return makeToken(TokenType::Not, "!", start);

      case '=':
         if (source_.peek() == '=') {
            source_.advance();
            return makeToken(TokenType::Equal, "==", start);
         }
         return makeToken(TokenType::Assign, "=", start);

      case '<':
         if (source_.peek() == '=') {
            source_.advance();
            return makeToken(TokenType::LessEqual, "<=", start);
         }
         return makeToken(TokenType::Less, "<", start);

      case '>':
         if (source_.peek() == '=') {
            source_.advance();
            return makeToken(TokenType::GreaterEqual, ">=", start);
         }
         return makeToken(TokenType::Greater, ">", start);

      case '&':
         if (source_.peek() == '&') {
            source_.advance();
            return makeToken(TokenType::LogicAnd, "&&", start);
         }
         return makeInvalidToken("&", start);

      case '|':
         if (source_.peek() == '|') {
            source_.advance();
            return makeToken(TokenType::LogicOr, "||", start);
         }
         if (source_.peek() == '>') {
            source_.advance();
            return makeToken(TokenType::OpMap, "|>", start);
         }
         return makeInvalidToken("|", start);

      case '"': {
         std::string text = "\"";

         while (!source_.isAtEnd() && source_.peek() != '"') {
            char ch = source_.advance();

            if (ch == '\n') {
               return makeInvalidToken(text, start);
            }

            text.push_back(ch);
         }

         if (source_.isAtEnd()) {
            return makeInvalidToken(text, start);
         }

         text.push_back(source_.advance());
         return makeToken(TokenType::StringLiteral, text, start);
      }

      case '\'': {
         std::string text = "'";

         if (source_.isAtEnd() || source_.peek() == '\n' || source_.peek() == '\'') {
            return makeInvalidToken(text, start);
         }

         text.push_back(source_.advance());

         if (source_.peek() != '\'') {
            return makeInvalidToken(text, start);
         }

         text.push_back(source_.advance());
         return makeToken(TokenType::CharLiteral, text, start);
      }
   }

   if (isDigit(c)) {
      std::string text;
      text.push_back(c);

      bool lastWasUnderscore = false;

      auto readDigitSequenceRest = [&]() -> bool {
         while (!source_.isAtEnd()) {
            char p = source_.peek();

            if (isDigit(p)) {
               text.push_back(source_.advance());
               lastWasUnderscore = false;
            } else if (p == '_') {
               text.push_back(source_.advance());

               if (lastWasUnderscore || !isDigit(source_.peek())) {
                  char c = source_.advance();
                  while(c == '_' || c == '.' || isDigit(c)){
                     text.push_back(c);
                     c = source_.advance();
                  }

                  return false;
               }

               lastWasUnderscore = true;
            } else {
               break;
            }
         }

         return !lastWasUnderscore;
      };

      if (!readDigitSequenceRest()) {
         return makeInvalidToken(text, start);
      }

      bool isFloat = false;

      if (source_.peek() == '.' && isDigit(source_.peekNext())) {
         isFloat = true;
         text.push_back(source_.advance());

         if (!isDigit(source_.peek())) {
            return makeInvalidToken(text, start);
         }

         text.push_back(source_.advance());
         lastWasUnderscore = false;

         if (!readDigitSequenceRest()) {
            return makeInvalidToken(text, start);
         }
      }

      return makeToken(isFloat ? TokenType::FloatLiteral : TokenType::IntLiteral, text, start);
   }

   if (isAlpha(c)) {
      std::string text;
      text.push_back(c);

      while (!source_.isAtEnd() && isAlphaNumericOrUnderscore(source_.peek())) {
         text.push_back(source_.advance());
      }

      return makeToken(keywordType(text), text, start);
   }

   return makeInvalidToken(std::string(1, c), start);
}