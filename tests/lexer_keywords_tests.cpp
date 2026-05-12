#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"
#include <string>

TEST(LexerKeywords, KeywordsAndTypes) {
   const std::string code =
      "class User {\n"
      "fun main(args: list<string>) -> int {\n"
      "return 0\n"
      "}\n"
      "}\n";

   const auto tokens = tokenizeString("keywords.djm", code);

   assertToken(tokens[0], TokenType::KwClass, "class", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "User", 1, 7);
   assertToken(tokens[2], TokenType::LBrace, "{", 1, 12);
   assertToken(tokens[3], TokenType::Newline, "\\n", 1, 13);

   assertToken(tokens[4], TokenType::KwFun, "fun", 2, 1);
   assertToken(tokens[5], TokenType::Identifier, "main", 2, 5);
   assertToken(tokens[6], TokenType::LParen, "(", 2, 9);
   assertToken(tokens[7], TokenType::Identifier, "args", 2, 10);
   assertToken(tokens[8], TokenType::Colon, ":", 2, 14);
   assertToken(tokens[9], TokenType::KwList, "list", 2, 16);
   assertToken(tokens[10], TokenType::Less, "<", 2, 20);
   assertToken(tokens[11], TokenType::KwString, "string", 2, 21);
   assertToken(tokens[12], TokenType::Greater, ">", 2, 27);
   assertToken(tokens[13], TokenType::RParen, ")", 2, 28);
   assertToken(tokens[14], TokenType::Arrow, "->", 2, 30);
   assertToken(tokens[15], TokenType::KwInt, "int", 2, 33);
   assertToken(tokens[16], TokenType::LBrace, "{", 2, 37);
   assertToken(tokens[17], TokenType::Newline, "\\n", 2, 38);

   assertToken(tokens[18], TokenType::KwReturn, "return", 3, 1);
   assertToken(tokens[19], TokenType::IntLiteral, "0", 3, 8);
   assertToken(tokens[20], TokenType::Newline, "\\n", 3, 9);

   assertToken(tokens[21], TokenType::RBrace, "}", 4, 1);
   assertToken(tokens[22], TokenType::Newline, "\\n", 4, 2);

   assertToken(tokens[23], TokenType::RBrace, "}", 5, 1);
   assertToken(tokens[24], TokenType::Newline, "\\n", 5, 2);

   assertToken(tokens[25], TokenType::EndOfFile, "", 6, 1);
}

TEST(LexerKeywords, AllSimpleKeywords) {
   const std::string code =
      "int uint float bool char string list void\n"
      "class fun return mut private static\n"
      "if else while for in\n"
      "import from this\n"
      "as contains count reverse flatten\n";

   const auto tokens = tokenizeString("all_keywords.djm", code);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::KwUint, "uint", 1, 5);
   assertToken(tokens[2], TokenType::KwFloat, "float", 1, 10);
   assertToken(tokens[3], TokenType::KwBool, "bool", 1, 16);
   assertToken(tokens[4], TokenType::KwChar, "char", 1, 21);
   assertToken(tokens[5], TokenType::KwString, "string", 1, 26);
   assertToken(tokens[6], TokenType::KwList, "list", 1, 33);
   assertToken(tokens[7], TokenType::KwVoid, "void", 1, 38);
   assertToken(tokens[8], TokenType::Newline, "\\n", 1, 42);

   assertToken(tokens[9], TokenType::KwClass, "class", 2, 1);
   assertToken(tokens[10], TokenType::KwFun, "fun", 2, 7);
   assertToken(tokens[11], TokenType::KwReturn, "return", 2, 11);
   assertToken(tokens[12], TokenType::KwMut, "mut", 2, 18);
   assertToken(tokens[13], TokenType::KwPrivate, "private", 2, 22);
   assertToken(tokens[14], TokenType::KwStatic, "static", 2, 30);
   assertToken(tokens[15], TokenType::Newline, "\\n", 2, 36);

   assertToken(tokens[16], TokenType::KwIf, "if", 3, 1);
   assertToken(tokens[17], TokenType::KwElse, "else", 3, 4);
   assertToken(tokens[18], TokenType::KwWhile, "while", 3, 9);
   assertToken(tokens[19], TokenType::KwFor, "for", 3, 15);
   assertToken(tokens[20], TokenType::KwIn, "in", 3, 19);
   assertToken(tokens[21], TokenType::Newline, "\\n", 3, 21);

   assertToken(tokens[22], TokenType::KwImport, "import", 4, 1);
   assertToken(tokens[23], TokenType::KwFrom, "from", 4, 8);
   assertToken(tokens[24], TokenType::KwThis, "this", 4, 13);
   assertToken(tokens[25], TokenType::Newline, "\\n", 4, 17);

   assertToken(tokens[26], TokenType::OpAs, "as", 5, 1);
   assertToken(tokens[27], TokenType::OpContains, "contains", 5, 4);
   assertToken(tokens[28], TokenType::OpCount, "count", 5, 13);
   assertToken(tokens[29], TokenType::OpReverse, "reverse", 5, 19);
   assertToken(tokens[30], TokenType::OpFlatten, "flatten", 5, 27);
   assertToken(tokens[31], TokenType::Newline, "\\n", 5, 34);

   assertToken(tokens[32], TokenType::EndOfFile, "", 6, 1);
}