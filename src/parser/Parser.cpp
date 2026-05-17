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

