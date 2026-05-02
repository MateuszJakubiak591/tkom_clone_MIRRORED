#pragma once

#include <string>
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

   KwClass,
   KwFun,
   KwReturn,
   KwMut,
   KwPrivate,
   KwStatic,

   KwIf,
   KwElse,
   KwWhile,
   KwFor,
   KwIn,

   KwImport,
   KwFrom,

   KwThis,

   OpAs,
   OpContains,
   OpCount,
   OpReverse,
   OpFlatten,
   OpMap,         // |>
   OpFilter,      // ?
   OpGroup,       // %

   Plus,          // +
   Minus,         // -
   Multiply,      // *
   Divide,         // /
   Power,         // ^
   Not,           // !

   Equal,         // ==
   NotEqual,      // !=
   Less,          // <
   LessEqual,     // <=
   Greater,       // >
   GreaterEqual,  // >=

   LogicAnd,      // &&
   LogicOr,       // ||

   Assign,        // =
   Arrow,         // ->

   LParen,        // (
   RParen,        // )
   LBracket,      // [
   RBracket,      // ]
   LBrace,        // {
   RBrace,        // }
   Comma,         // ,
   Dot,           // .
   Colon          // :
};

struct Token {
   TokenType type;
   std::string lexeme;
   SourceLocation location;
};

std::string tokenTypeToString(TokenType type);

Token makeToken(TokenType type, const std::string& lexeme, 
                              const SourceLocation& location);

Token makeInvalidToken(const std::string& lexeme,
                     const SourceLocation& location);

TokenType keywordType(const std::string& text);

// Przyjęto taką definicję dla celów testowych
// Faktycznie należałoby pewnie porównać
// jedynie type i lexeme
inline bool operator==(const Token& lhs, const Token& rhs) {
   return lhs.type == rhs.type
       && lhs.lexeme == rhs.lexeme
       && lhs.location == rhs.location;
}
