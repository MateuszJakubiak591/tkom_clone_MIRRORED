#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"
#include <string>

TEST(LexerKeywords, KeywordsAndTypes) {
   const std::string code =
      ""
      "fun main(args: list<string>) -> int {\n"
      "return 0\n"
      "}\n";

   const auto tokens = tokenizeString("keywords.djm", code);

   assertToken(tokens[0], TokenType::KwFun, "fun", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "main", 1, 5);
   assertToken(tokens[2], TokenType::LParen, "(", 1, 9);
   assertToken(tokens[3], TokenType::Identifier, "args", 1, 10);
   assertToken(tokens[4], TokenType::Colon, ":", 1, 14);
   assertToken(tokens[5], TokenType::KwList, "list", 1, 16);
   assertToken(tokens[6], TokenType::Less, "<", 1, 20);
   assertToken(tokens[7], TokenType::KwString, "string", 1, 21);
   assertToken(tokens[8], TokenType::Greater, ">", 1, 27);
   assertToken(tokens[9], TokenType::RParen, ")", 1, 28);
   assertToken(tokens[10], TokenType::Arrow, "->", 1, 30);
   assertToken(tokens[11], TokenType::KwInt, "int", 1, 33);
   assertToken(tokens[12], TokenType::LBrace, "{", 1, 37);
   assertToken(tokens[13], TokenType::Newline, "\\n", 1, 38);

   assertToken(tokens[14], TokenType::KwReturn, "return", 2, 1);
   assertToken(tokens[15], TokenType::IntLiteral, "0", 2, 8);
   assertToken(tokens[16], TokenType::Newline, "\\n", 2, 9);

   assertToken(tokens[17], TokenType::RBrace, "}", 3, 1);
   assertToken(tokens[18], TokenType::Newline, "\\n", 3, 2);

   assertToken(tokens[19], TokenType::EndOfFile, "", 4, 1);
}

TEST(LexerKeywords, AllSimpleKeywords) {
   const std::string code =
      "int uint float bool char string list void\n"
      "fun return mut\n"
      "if else while for in\n"
      "import from this\n"
      "as contains count reverse flatten\n"
      "while break continue\n";
      

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

   assertToken(tokens[9], TokenType::KwFun, "fun", 2, 1);
   assertToken(tokens[10], TokenType::KwReturn, "return", 2, 5);
   assertToken(tokens[11], TokenType::KwMut, "mut", 2, 12);
   assertToken(tokens[12], TokenType::Newline, "\\n", 2, 15);

   assertToken(tokens[13], TokenType::KwIf, "if", 3, 1);
   assertToken(tokens[14], TokenType::KwElse, "else", 3, 4);
   assertToken(tokens[15], TokenType::KwWhile, "while", 3, 9);
   assertToken(tokens[16], TokenType::KwFor, "for", 3, 15);
   assertToken(tokens[17], TokenType::KwIn, "in", 3, 19);
   assertToken(tokens[18], TokenType::Newline, "\\n", 3, 21);

   assertToken(tokens[19], TokenType::KwImport, "import", 4, 1);
   assertToken(tokens[20], TokenType::KwFrom, "from", 4, 8);
   assertToken(tokens[21], TokenType::KwThis, "this", 4, 13);
   assertToken(tokens[22], TokenType::Newline, "\\n", 4, 17);

   assertToken(tokens[23], TokenType::OpAs, "as", 5, 1);
   assertToken(tokens[24], TokenType::OpContains, "contains", 5, 4);
   assertToken(tokens[25], TokenType::OpCount, "count", 5, 13);
   assertToken(tokens[26], TokenType::OpReverse, "reverse", 5, 19);
   assertToken(tokens[27], TokenType::OpFlatten, "flatten", 5, 27);
   assertToken(tokens[28], TokenType::Newline, "\\n", 5, 34);

   assertToken(tokens[29], TokenType::KwWhile, "while", 6, 1);
   assertToken(tokens[30], TokenType::KwBreak, "break", 6, 7);
   assertToken(tokens[31], TokenType::KwContinue, "continue", 6, 13);
   assertToken(tokens[32], TokenType::Newline, "\\n", 6, 21);

   assertToken(tokens[33], TokenType::EndOfFile, "", 7, 1);
}