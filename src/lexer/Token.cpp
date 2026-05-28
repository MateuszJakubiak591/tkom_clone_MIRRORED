#include "lexer/Token.hpp"
#include <unordered_map>
#include <stdexcept>

Token::Token(
   TokenType type,
   std::string lexeme,
   SourceLocation location,
   TokenValue value
)
   : type_(type),
     lexeme_(std::move(lexeme)),
     location_(location),
     value_(std::move(value)) {
   if (!isConsistent(type_, lexeme_, value_)) {
      throw std::invalid_argument(
         "Inconsistent token: type " + tokenTypeToString(type_) +
         " with lexeme '" + lexeme_ + "'"
      );
   }
}

TokenType Token::type() const {
   return type_;
}

const std::string& Token::lexeme() const {
   return lexeme_;
}

const SourceLocation& Token::location() const {
   return location_;
}

const TokenValue& Token::value() const {
   return value_;
}

bool Token::hasValue() const {
   return !std::holds_alternative<std::monostate>(value_);
}

bool Token::isConsistent(
   TokenType type,
   const std::string& lexeme,
   const TokenValue& value
) {
   switch (type) {
      case TokenType::IntLiteral:
         return std::holds_alternative<int64_t>(value);

      case TokenType::FloatLiteral:
         return std::holds_alternative<double>(value);

      case TokenType::BoolLiteral:
         return std::holds_alternative<bool>(value) &&
                (lexeme == "true" || lexeme == "false");

      case TokenType::CharLiteral:
         return std::holds_alternative<char>(value);

      case TokenType::StringLiteral:
         return std::holds_alternative<std::string>(value);

      case TokenType::Invalid:
         return true;

      default:
         return std::holds_alternative<std::monostate>(value);
   }
}

Token makeToken(TokenType type, const std::string& lexeme, const SourceLocation& location) {
   return Token(type, lexeme, location);
}

Token makeInvalidToken(const std::string& lexeme, const SourceLocation& location) {
   return Token(TokenType::Invalid, lexeme, location);
}

Token makeIntToken(const std::string& lexeme, SourceLocation location, int64_t value) {
   return Token(TokenType::IntLiteral, lexeme, location, value);
}

Token makeFloatToken(const std::string& lexeme, SourceLocation location, double value) {
   return Token(TokenType::FloatLiteral, lexeme, location, value);
}

Token makeBoolToken(const std::string& lexeme, SourceLocation location, bool value) {
   return Token(TokenType::BoolLiteral, lexeme, location, value);
}

Token makeCharToken(const std::string& lexeme, SourceLocation location, char value) {
   return Token(TokenType::CharLiteral, lexeme, location, value);
}

Token makeStringToken(const std::string& lexeme, SourceLocation location, std::string value) {
   return Token(TokenType::StringLiteral, lexeme, location, std::move(value));
}

#include <string_view>
#include <iterator> 

TokenType keywordType(const std::string& text) {
   static constexpr std::pair<std::string_view, TokenType> keywords[] = {
      {"int", TokenType::KwInt},
      {"uint", TokenType::KwUint},
      {"float", TokenType::KwFloat},
      {"bool", TokenType::KwBool},
      {"char", TokenType::KwChar},
      {"string", TokenType::KwString},
      {"list", TokenType::KwList},
      {"void", TokenType::KwVoid},

      {"fun", TokenType::KwFun},
      {"return", TokenType::KwReturn},
      {"mut", TokenType::KwMut},

      {"if", TokenType::KwIf},
      {"else", TokenType::KwElse},
      {"while", TokenType::KwWhile},
      {"for", TokenType::KwFor},
      {"in", TokenType::KwIn},
      {"break", TokenType::KwBreak},
      {"continue", TokenType::KwContinue},

      {"import", TokenType::KwImport},
      {"from", TokenType::KwFrom},
      {"this", TokenType::KwThis},

      {"true", TokenType::BoolLiteral},
      {"false", TokenType::BoolLiteral},

      {"as", TokenType::OpAs},
      {"contains", TokenType::OpContains},
      {"count", TokenType::OpCount},
      {"reverse", TokenType::OpReverse},
      {"flatten", TokenType::OpFlatten}
   };

   for (const auto& [kw, type] : keywords) {
      if (kw == text) {
         return type;
      }
   }

   return TokenType::Identifier;
}

std::string tokenTypeToString(TokenType type) {
   switch (type) {
      case TokenType::EndOfFile: return "EndOfFile";
      case TokenType::Invalid: return "Invalid";
      case TokenType::Newline: return "Newline";

      case TokenType::IntLiteral: return "IntLiteral";
      case TokenType::FloatLiteral: return "FloatLiteral";
      case TokenType::StringLiteral: return "StringLiteral";
      case TokenType::CharLiteral: return "CharLiteral";
      case TokenType::BoolLiteral: return "BoolLiteral";

      case TokenType::Identifier: return "Identifier";

      case TokenType::KwInt: return "KwInt";
      case TokenType::KwUint: return "KwUint";
      case TokenType::KwFloat: return "KwFloat";
      case TokenType::KwBool: return "KwBool";
      case TokenType::KwChar: return "KwChar";
      case TokenType::KwString: return "KwString";
      case TokenType::KwList: return "KwList";
      case TokenType::KwVoid: return "KwVoid";

      case TokenType::KwFun: return "KwFun";
      case TokenType::KwReturn: return "KwReturn";
      case TokenType::KwMut: return "KwMut";

      case TokenType::KwIf: return "KwIf";
      case TokenType::KwElse: return "KwElse";
      case TokenType::KwWhile: return "KwWhile";
      case TokenType::KwFor: return "KwFor";
      case TokenType::KwIn: return "KwIn";

      case TokenType::KwImport: return "KwImport";
      case TokenType::KwFrom: return "KwFrom";
      case TokenType::KwThis: return "KwThis";

      case TokenType::OpAs: return "OpAs";
      case TokenType::OpContains: return "OpContains";
      case TokenType::OpCount: return "OpCount";
      case TokenType::OpReverse: return "OpReverse";
      case TokenType::OpFlatten: return "OpFlatten";
      case TokenType::OpMap: return "OpMap";
      case TokenType::OpFilter: return "OpFilter";
      case TokenType::OpGroup: return "OpGroup";

      case TokenType::Plus: return "Plus";
      case TokenType::Minus: return "Minus";
      case TokenType::Multiply: return "Multiply";
      case TokenType::Divide: return "Divide";
      case TokenType::Power: return "Caret";
      case TokenType::Not: return "Not";

      case TokenType::Equal: return "Equal";
      case TokenType::NotEqual: return "NotEqual";
      case TokenType::Less: return "Less";
      case TokenType::LessEqual: return "LessEqual";
      case TokenType::Greater: return "Greater";
      case TokenType::GreaterEqual: return "GreaterEqual";

      case TokenType::LogicAnd: return "LogicAnd";
      case TokenType::LogicOr: return "LogicOr";

      case TokenType::Assign: return "Assign";
      case TokenType::Arrow: return "Arrow";

      case TokenType::LParen: return "LParen";
      case TokenType::RParen: return "RParen";
      case TokenType::LBracket: return "LBracket";
      case TokenType::RBracket: return "RBracket";
      case TokenType::LBrace: return "LBrace";
      case TokenType::RBrace: return "RBrace";
      case TokenType::Comma: return "Comma";
      case TokenType::Dot: return "Dot";
      case TokenType::Colon: return "Colon";
      case TokenType::KwBreak: return "KwBreak";
      case TokenType::KwContinue: return "KwContinue";
   }

   return "Unknown";
}