#include "lexer/Lexer.hpp"

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

#include "lexer/CharBasicLib.hpp"
#include "lexer/Token.hpp"

namespace {

std::string removeUnderscores(const std::string& text) {
   std::string result;
   result.reserve(text.size());

   for (char c : text) {
      if (c != '_') {
         result.push_back(c);
      }
   }

   return result;
}

int64_t parseIntLiteral(const std::string& text) {
   std::string normalized = removeUnderscores(text);
   
   int64_t result = 0;
   bool negative = false;
   size_t start = 0;

   if (!normalized.empty() && normalized[0] == '-') {
      negative = true;
      start = 1;
   }

   for (size_t i = start; i < normalized.size(); ++i) {
      if (normalized[i] < '0' || normalized[i] > '9') {
         throw std::invalid_argument("Niedozwolony znak w liczbie");
      }

      int digit = normalized[i] - '0';

      //if (result * 10 + digit > INT64_MAX)
      //if (result * 10 > INT64_MAX - digit)
      //if (result > (INT64_MAX - digit) / 10)
      if (result > (INT64_MAX - digit) / 10) {
         throw std::out_of_range("Liczba wykracza poza zakres int64");
      }

      result = result * 10 + digit;
   }

   return negative ? -result : result;
}

double parseFloatLiteral(const std::string& text) {
   std::string normalized = removeUnderscores(text);
   
   double result = 0.0;
   size_t i = 0;
   bool negative = false;

   if (!normalized.empty() && normalized[0] == '-') {
      negative = true;
      i = 1;
   }

   while (i < normalized.size() && normalized[i] != '.') {
      result = result * 10.0 + (normalized[i] - '0');
      i++;
   }

   if (i < normalized.size() && normalized[i] == '.') {
      i++;
      double weight = 0.1;
      while (i < normalized.size()) {
         result += (normalized[i] - '0') * weight;
         weight /= 10.0;
         i++;
      }
   }

   return negative ? -result : result;
}

bool isValidSimpleEscape(char c) {
   return c == 'n' ||
          c == 't' ||
          c == 'r' ||
          c == '\\' ||
          c == '"' ||
          c == '\'';
}

char decodeSimpleEscape(char c) {
   switch (c) {
      case 'n': return '\n';
      case 't': return '\t';
      case 'r': return '\r';
      case '\\': return '\\';
      case '"': return '"';
      case '\'': return '\'';

      default:
         throw std::invalid_argument("invalid escape sequence");
   }
}

} // namespace

Lexer::Lexer(CharSource& source)
   : source_(source) {}

Token Lexer::nextToken() {
   if (auto token = skipIgnored()) {
      return *token;
   }

   SourceLocation start = source_.currentLocation();

   if (source_.isAtEnd()) {
      return makeToken(TokenType::EndOfFile, "", start);
   }

   char c = source_.advance();

   if (auto token = readSymbolOrOperator(c, start)) {
      return *token;
   }

   if (c == '"') {
      return readStringLiteral(start);
   }

   if (c == '\'') {
      return readCharLiteral(start);
   }

   if (isDigit(c)) {
      return readNumberLiteral(c, start);
   }

   if (isAlpha(c)) {
      return readIdentifierOrKeyword(c, start);
   }

   return makeInvalidToken(std::string(1, c), start);
}

std::optional<Token> Lexer::skipIgnored() {
   while (!source_.isAtEnd()) {
      char c = source_.peek();

      if (isWhiteSpace(c)) {
         skipWhitespace();
         continue;
      }

      if (c == '/' && source_.peekNext() == '/') {
         skipLineComment();
         continue;
      }

      if (c == '/' && source_.peekNext() == '*') {
         if (auto token = skipBlockComment()) {
            return token;
         }

         continue;
      }

      if (c == '\n') {
         return readNewline();
      }

      break;
   }

   return std::nullopt;
}

void Lexer::skipWhitespace() {
   while (!source_.isAtEnd() && isWhiteSpace(source_.peek())) {
      source_.advance();
   }
}

void Lexer::skipLineComment() {
   while (!source_.isAtEnd() && source_.peek() != '\n') {
      source_.advance();
   }
}

std::optional<Token> Lexer::skipBlockComment() {
   SourceLocation start = source_.currentLocation();

   source_.advance(); // /
   source_.advance(); // *

   while (!source_.isAtEnd()) {
      if (source_.peek() == '*' && source_.peekNext() == '/') {
         source_.advance(); // *
         source_.advance(); // /
         return std::nullopt;
      }

      source_.advance();
   }

   return makeInvalidToken("unterminated block comment", start);
}

std::optional<Token> Lexer::readNewline() {
   SourceLocation start = source_.currentLocation();
   source_.advance();

   if (parenDepth_ > 0 || bracketDepth_ > 0) {
      return std::nullopt;
   }

   return makeToken(TokenType::Newline, "\\n", start);
}

std::optional<Token> Lexer::readSymbolOrOperator(char c, SourceLocation start) {
   switch (c) {
      case '(':
         ++parenDepth_;
         return makeToken(TokenType::LParen, "(", start);

      case ')':
         if (parenDepth_ > 0) {
            --parenDepth_;
         }
         return makeToken(TokenType::RParen, ")", start);

      case '[':
         ++bracketDepth_;
         return makeToken(TokenType::LBracket, "[", start);

      case ']':
         if (bracketDepth_ > 0) {
            --bracketDepth_;
         }
         return makeToken(TokenType::RBracket, "]", start);

      case '{':
         return makeToken(TokenType::LBrace, "{", start);

      case '}':
         return makeToken(TokenType::RBrace, "}", start);

      case ',':
         return makeToken(TokenType::Comma, ",", start);

      case '.':
         return makeToken(TokenType::Dot, ".", start);

      case ':':
         return makeToken(TokenType::Colon, ":", start);

      case '?':
         return makeToken(TokenType::OpFilter, "?", start);

      case '%':
         return makeToken(TokenType::OpGroup, "%", start);

      case '+':
         return makeToken(TokenType::Plus, "+", start);

      case '*':
         return makeToken(TokenType::Multiply, "*", start);

      case '/':
         return makeToken(TokenType::Divide, "/", start);

      case '^':
         return makeToken(TokenType::Power, "^", start);

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

      default:
         return std::nullopt;
   }
}

Token Lexer::readStringLiteral(SourceLocation start) {
   std::string lexeme = "\"";
   std::string value;

   while (!source_.isAtEnd() && source_.peek() != '"') {
      char c = source_.peek();

      if (c == '\n') {
         return makeInvalidToken(lexeme, start);
      }

      source_.advance();
      lexeme.push_back(c);

      if (c == '\\') {
         if (source_.isAtEnd()) {
            return makeInvalidToken(lexeme, start);
         }

         char escaped = source_.advance();
         lexeme.push_back(escaped);

         if (!isValidSimpleEscape(escaped)) {
            return makeInvalidToken(lexeme, start);
         }

         value.push_back(decodeSimpleEscape(escaped));
      } else {
         value.push_back(c);
      }
   }

   if (source_.isAtEnd()) {
      return makeInvalidToken(lexeme, start);
   }

   lexeme.push_back(source_.advance()); // "

   return makeStringToken(lexeme, start, value);
}

Token Lexer::readCharLiteral(SourceLocation start) {
   std::string lexeme = "'";
   char value = '\0';

   if (source_.isAtEnd() || source_.peek() == '\n') {
      return makeInvalidToken(lexeme, start);
   }

   char c = source_.advance();
   lexeme.push_back(c);

   if (c == '\\') {
      if (source_.isAtEnd()) {
         return makeInvalidToken(lexeme, start);
      }

      char escaped = source_.advance();
      lexeme.push_back(escaped);

      if (!isValidSimpleEscape(escaped)) {
         return makeInvalidToken(lexeme, start);
      }

      value = decodeSimpleEscape(escaped);
   } else {
      if (c == '\'') {
         return makeInvalidToken(lexeme, start);
      }

      value = c;
   }

   if (source_.isAtEnd() || source_.peek() != '\'') {
      return makeInvalidToken(lexeme, start);
   }

   lexeme.push_back(source_.advance()); // '

   return makeCharToken(lexeme, start, value);
}

Token Lexer::readNumberLiteral(char first, SourceLocation start) {
   std::string lexeme;
   lexeme.push_back(first);

   char lastChar = first;
   bool hasDot = false;
   bool isError = false;

   while (!source_.isAtEnd()) {
      char c = source_.peek();

      if (isDigit(c)) {
         lexeme.push_back(source_.advance());
         lastChar = c;
      } 
      else if (c == '_') {
         if (lastChar == '_') isError = true;
         if (lastChar == '.') isError = true;

         lexeme.push_back(source_.advance());
         lastChar = c;
      } 
      else if (c == '.') {
         if (hasDot) break;
         if (lastChar == '_') isError = true;

         if (!isDigit(source_.peekNext())) {
            break; 
         }

         lexeme.push_back(source_.advance());
         lastChar = c;
         hasDot = true;
      } 
      else if (isAlpha(c)) {
         isError = true;
         lexeme.push_back(source_.advance());
         lastChar = c;
      } 
      else {
         break;
      }
   }

   if (lastChar == '_') {
      isError = true;
   }

   if (isError) {
      while (!source_.isAtEnd() && (isAlphaNumeric(source_.peek()) || source_.peek() == '_' || source_.peek() == '.')) {
         lexeme.push_back(source_.advance());
      }
      return makeInvalidToken(lexeme, start);
   }

   try {
      if (hasDot) {
         return makeFloatToken(lexeme, start, parseFloatLiteral(lexeme));
      }
      return makeIntToken(lexeme, start, parseIntLiteral(lexeme));
   } catch (const std::exception&) {
      return makeInvalidToken(lexeme, start);
   }
}

Token Lexer::readIdentifierOrKeyword(char first, SourceLocation start) {
   std::string lexeme;
   lexeme.push_back(first);

   while (!source_.isAtEnd() && isAlphaNumericOrUnderscore(source_.peek())) {
      lexeme.push_back(source_.advance());
   }

   TokenType type = keywordType(lexeme);

   if (type == TokenType::BoolLiteral) {
      return makeBoolToken(lexeme, start, lexeme == "true");
   }

   return makeToken(type, lexeme, start);
}