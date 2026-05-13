#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"
#include <string>

TEST(LexerStrings, StringsAndChars) {
   const std::string code =
      "string s = \"abc\"\n"
      "char c = 'x'\n";

   const auto tokens = tokenizeString("strings.djm", code);

   assertToken(tokens[0], TokenType::KwString, "string", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "s", 1, 8);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 10);
   assertToken(tokens[3], TokenType::StringLiteral, "\"abc\"", 1, 12);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 17);

   assertToken(tokens[5], TokenType::KwChar, "char", 2, 1);
   assertToken(tokens[6], TokenType::Identifier, "c", 2, 6);
   assertToken(tokens[7], TokenType::Assign, "=", 2, 8);
   assertToken(tokens[8], TokenType::CharLiteral, "'x'", 2, 10);
   assertToken(tokens[9], TokenType::Newline, "\\n", 2, 13);

   assertToken(tokens[10], TokenType::EndOfFile, "", 3, 1);
}

TEST(LexerStrings, StringsWithSpaces) {
   const std::string code =
      "string message = \"hello world\"\n";

   const auto tokens = tokenizeString("string_spaces.djm", code);

   assertToken(tokens[0], TokenType::KwString, "string", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "message", 1, 8);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 16);
   assertToken(tokens[3], TokenType::StringLiteral, "\"hello world\"", 1, 18);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 31);
   assertToken(tokens[5], TokenType::EndOfFile, "", 2, 1);
}

TEST(LexerStrings, InvalidUntermiantedStrings)  {
   const std::string code =
      "string s = \"abc\n";
   
   const auto tokens = tokenizeString("invalid_string.djm", code);

   assertToken(tokens[0], TokenType::KwString, "string", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "s", 1, 8);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 10);


   assertToken(tokens[3], TokenType::Invalid, "\"abc", 1, 12);
   EXPECT_EQ(tokens[2].hasValue(), false);
}

TEST(LexerStrings, StringWithEscapes) {
   const std::string code =
      "string s = \"line1\\nline2 \\\"quote\\\"\"\n";

   const auto tokens = tokenizeString("string_escapes.djm", code);

   assertToken(tokens[0], TokenType::KwString, "string", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "s", 1, 8);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 10);

   assertToken(tokens[3], TokenType::StringLiteral, "\"line1\\nline2 \\\"quote\\\"\"", 1, 12);

   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 36);
   
   assertToken(tokens[5], TokenType::EndOfFile, "", 2, 1);
}