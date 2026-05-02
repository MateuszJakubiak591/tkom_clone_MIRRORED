#include "lexer/Token.hpp"
#include <unordered_map>

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

      case TokenType::KwClass: return "KwClass";
      case TokenType::KwFun: return "KwFun";
      case TokenType::KwReturn: return "KwReturn";
      case TokenType::KwMut: return "KwMut";
      case TokenType::KwPrivate: return "KwPrivate";
      case TokenType::KwStatic: return "KwStatic";

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
   }

   return "Unknown";
}

Token makeToken(TokenType type, const std::string& lexeme, const SourceLocation& location) {
   return Token{type, lexeme, location};
}

Token makeInvalidToken(const std::string& lexeme, const SourceLocation& location) {
   return Token{TokenType::Invalid, lexeme, location};
}

TokenType keywordType(const std::string& text) {
   static const std::unordered_map<std::string, TokenType> keywords = {
      {"int", TokenType::KwInt},
      {"uint", TokenType::KwUint},
      {"float", TokenType::KwFloat},
      {"bool", TokenType::KwBool},
      {"char", TokenType::KwChar},
      {"string", TokenType::KwString},
      {"list", TokenType::KwList},
      {"void", TokenType::KwVoid},

      {"class", TokenType::KwClass},
      {"fun", TokenType::KwFun},
      {"return", TokenType::KwReturn},
      {"mut", TokenType::KwMut},
      {"private", TokenType::KwPrivate},
      {"static", TokenType::KwStatic},

      {"if", TokenType::KwIf},
      {"else", TokenType::KwElse},
      {"while", TokenType::KwWhile},
      {"for", TokenType::KwFor},
      {"in", TokenType::KwIn},

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

   auto it = keywords.find(text);
   if (it != keywords.end()) {
      return it->second;
   }

   return TokenType::Identifier;
}