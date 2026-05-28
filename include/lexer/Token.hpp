#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "source/SourceLocation.hpp"

enum class TokenType {
   EndOfFile,
   Invalid,
   Newline,

   IntLiteral,
   FloatLiteral,
   StringLiteral,
   CharLiteral,
   BoolLiteral,

   Identifier,

   KwInt,
   KwUint,
   KwFloat,
   KwBool,
   KwChar,
   KwString,
   KwList,
   KwVoid,

   KwFun,
   KwReturn,
   KwMut,

   KwIf,
   KwElse,
   KwWhile,
   KwFor,
   KwIn,

   KwBreak,
   KwContinue,

   KwImport,
   KwFrom,
   KwThis,

   OpAs,
   OpContains,
   OpCount,
   OpReverse,
   OpFlatten,
   OpMap,
   OpFilter,
   OpGroup,

   Plus,
   Minus,
   Multiply,
   Divide,
   Power,
   Not,

   Equal,
   NotEqual,
   Less,
   LessEqual,
   Greater,
   GreaterEqual,

   LogicAnd,
   LogicOr,

   Assign,
   Arrow,

   LParen,
   RParen,
   LBracket,
   RBracket,
   LBrace,
   RBrace,
   Comma,
   Dot,
   Colon
};

using TokenValue = std::variant<
   std::monostate,
   int64_t,
   double,
   bool,
   char,
   std::string
>;

class Token {
public:
   Token(
      TokenType type,
      std::string lexeme,
      SourceLocation location,
      TokenValue value = std::monostate{}
   );

   TokenType type() const;
   const std::string& lexeme() const;
   const SourceLocation& location() const;
   const TokenValue& value() const;

   bool hasValue() const;

private:
   TokenType type_;
   std::string lexeme_;
   SourceLocation location_;
   TokenValue value_;

   static bool isConsistent(TokenType type, const std::string& lexeme, const TokenValue& value);
};



Token makeToken(TokenType type, const std::string& lexeme, const SourceLocation& location);
Token makeInvalidToken(const std::string& lexeme, const SourceLocation& location);

Token makeIntToken(const std::string& lexeme, SourceLocation location, int64_t value);
Token makeFloatToken(const std::string& lexeme, SourceLocation location, double value);
Token makeBoolToken(const std::string& lexeme, SourceLocation location, bool value);
Token makeCharToken(const std::string& lexeme, SourceLocation location, char value);
Token makeStringToken(const std::string& lexeme, SourceLocation location, std::string value);

TokenType keywordType(const std::string& text);

std::string tokenTypeToString(TokenType type);