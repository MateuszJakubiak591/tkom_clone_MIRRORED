#include "parser/Parser.hpp"

#include <sstream>
#include <utility>

#include "diagnostics/ErrorHandler.hpp"
#include "diagnostics/Error.hpp"

Parser::Parser(Lexer& lexer, ErrorHandler* errorHandler)
   : lexer_(lexer),
     errorHandler_(errorHandler),
     current_(makeToken(TokenType::EndOfFile, "", SourceLocation{})),
     previous_(makeToken(TokenType::EndOfFile, "", SourceLocation{})) {
   advance();
}

void Parser::advance() {
   previous_ = current_;
   current_ = lexer_.nextToken();

   /*
   void report(
      ErrorType type,
      const std::string& message,
      const SourceLocation& location
   );*/
}

bool Parser::check(TokenType type) const {
   return current_.type() == type;
}

bool Parser::match(TokenType type) {
   if (!check(type)) {
      return false;
   }

   advance();
   return true;
}

const Token& Parser::consume(TokenType type, const std::string& message) {
   if (!check(type)) {
      if (errorHandler_ != nullptr) {
         errorHandler_->report(ErrorType::Parser, message, current_.location());
      }

      throw std::runtime_error(message); // !!! UWAGA CZY ZAWSZE NALEŻY? CHYBA NIE
                                          // CZASEM W GRAMATYCE SĄ OPCJONALNE TOKENY
   }

   const Token& token = current_;
   advance();
   return previous_;
}

bool Parser::isAtEnd() const {
   return current_.type() == TokenType::EndOfFile;
}

bool Parser::matchNewline() {
   return match(TokenType::Newline);
}

void Parser::skipNewlines() {
   while (match(TokenType::Newline)) {}
}

bool Parser::isBasicValueTypeStart() const {
return check(TokenType::KwInt)
      || check(TokenType::KwUint)
      || check(TokenType::KwFloat)
      || check(TokenType::KwBool)
      || check(TokenType::KwChar)
      || check(TokenType::KwString);
}

bool Parser::isValueTypeStart() const {
return isBasicValueTypeStart()
      || check(TokenType::KwList)
      || check(TokenType::Identifier);
}

std::unique_ptr<TypeNode> Parser::parseType() {
   // !!! Trzeba wyróżnić void, i nie umieszczać go
   // w parseValueType
   if (check(TokenType::KwVoid)) {
      SourceLocation location = current_.location();
      advance();
      return std::make_unique<VoidTypeNode>(location);
   }

   return parseValueType();
}

std::unique_ptr<TypeNode> Parser::parseValueType() {
   SourceLocation location = current_.location();

   if (match(TokenType::KwInt)) {
      return std::make_unique<IntTypeNode>(location);
   }

   if (match(TokenType::KwUint)) {
      return std::make_unique<UintTypeNode>(location);
   }

   if (match(TokenType::KwFloat)) {
      return std::make_unique<FloatTypeNode>(location);
   }

   if (match(TokenType::KwBool)) {
      return std::make_unique<BoolTypeNode>(location);
   }

   if (match(TokenType::KwChar)) {
      return std::make_unique<CharTypeNode>(location);
   }

   if (match(TokenType::KwString)) {
      return std::make_unique<StringTypeNode>(location);
   }

   if (match(TokenType::KwList)) {
      consume(TokenType::Less, "expected '<' after list");

      auto elementType = parseValueType();

      consume(TokenType::Greater, "expected '>' after list element type");

      return std::make_unique<ListTypeNode>(location, std::move(elementType));
   }

   if (check(TokenType::Identifier)) {
      std::string name = current_.lexeme();
      advance();

      return std::make_unique<UserTypeNode>(location, std::move(name));
   }

   errorHandler_->report(ErrorType::Parser, "expected value type", current_.location());
}