#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"
#include <string>
#include <vector>

namespace {
struct SingleTokenCase {
   std::string source;
   TokenType type;
};

void assertSingleToken(const SingleTokenCase& testCase) {
   const auto tokens = tokenizeString("single_token.djm", testCase.source);

   ASSERT_EQ(tokens.size(), 2);
   assertToken(tokens[0], testCase.type, testCase.source, 1, 1);
   assertToken(
      tokens[1],
      TokenType::EndOfFile,
      "",
      1,
      static_cast<int>(testCase.source.size()) + 1
   );
}
}

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

TEST(LexerKeywords, TokenizesEachKeywordInIsolation) {
   const std::vector<SingleTokenCase> cases = {
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
   };

   for (const auto& testCase : cases) {
      assertSingleToken(testCase);
   }
}

TEST(LexerKeywords, TokenizesEachWordOperatorInIsolation) {
   const std::vector<SingleTokenCase> cases = {
      {"as", TokenType::OpAs},
      {"contains", TokenType::OpContains},
      {"count", TokenType::OpCount},
      {"reverse", TokenType::OpReverse},
      {"flatten", TokenType::OpFlatten},
   };

   for (const auto& testCase : cases) {
      assertSingleToken(testCase);
   }
}
