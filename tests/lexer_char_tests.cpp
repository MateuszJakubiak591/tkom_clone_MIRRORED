#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"
#include <string>


TEST(LexerChars, StringsAndChars)  {
   const std::string code =
      "char c = ''\n";

   const auto tokens = tokenizeString("invalid_char.djm", code);

   assertToken(tokens[0], TokenType::KwChar, "char", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "c", 1, 6);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 8);
   assertToken(tokens[3], TokenType::Invalid, "''", 1, 10);
}

static void testValidSimpleCharLiterals() {
   const std::string code =
      "char a = 'x'\n"
      "char b = '7'\n"
      "char c = '_'\n"
      "char d = ' '\n"
      "char e = '!'\n"
      "char f = '\\'\n";

   const auto tokens = tokenizeString("valid_chars.djm", code);

   assertToken(tokens[0], TokenType::KwChar, "char", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "a", 1, 6);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 8);
   assertToken(tokens[3], TokenType::CharLiteral, "'x'", 1, 10);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 13);

   assertToken(tokens[5], TokenType::KwChar, "char", 2, 1);
   assertToken(tokens[6], TokenType::Identifier, "b", 2, 6);
   assertToken(tokens[7], TokenType::Assign, "=", 2, 8);
   assertToken(tokens[8], TokenType::CharLiteral, "'7'", 2, 10);
   assertToken(tokens[9], TokenType::Newline, "\\n", 2, 13);

   assertToken(tokens[10], TokenType::KwChar, "char", 3, 1);
   assertToken(tokens[11], TokenType::Identifier, "c", 3, 6);
   assertToken(tokens[12], TokenType::Assign, "=", 3, 8);
   assertToken(tokens[13], TokenType::CharLiteral, "'_'", 3, 10);
   assertToken(tokens[14], TokenType::Newline, "\\n", 3, 13);

   assertToken(tokens[15], TokenType::KwChar, "char", 4, 1);
   assertToken(tokens[16], TokenType::Identifier, "d", 4, 6);
   assertToken(tokens[17], TokenType::Assign, "=", 4, 8);
   assertToken(tokens[18], TokenType::CharLiteral, "' '", 4, 10);
   assertToken(tokens[19], TokenType::Newline, "\\n", 4, 13);

   assertToken(tokens[20], TokenType::KwChar, "char", 5, 1);
   assertToken(tokens[21], TokenType::Identifier, "e", 5, 6);
   assertToken(tokens[22], TokenType::Assign, "=", 5, 8);
   assertToken(tokens[23], TokenType::CharLiteral, "'!'", 5, 10);
   assertToken(tokens[24], TokenType::Newline, "\\n", 5, 13);

   assertToken(tokens[25], TokenType::KwChar, "char", 6, 1);
   assertToken(tokens[26], TokenType::Identifier, "f", 6, 6);
   assertToken(tokens[27], TokenType::Assign, "=", 6, 8);
   assertToken(tokens[28], TokenType::CharLiteral, "'\\'", 6, 10);
   assertToken(tokens[29], TokenType::Newline, "\\n", 6, 13);

   assertToken(tokens[30], TokenType::EndOfFile, "", 7, 1);
}

static void testInvalidUnterminatedCharAtEndOfFile() {
   const std::string code =
      "char c = 'x";

   const auto tokens = tokenizeString("invalid_unterminated_char_eof.djm", code);

   assertToken(tokens[0], TokenType::KwChar, "char", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "c", 1, 6);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 8);
   assertToken(tokens[3], TokenType::Invalid, "'x", 1, 10);
}

static void testInvalidUnterminatedCharBeforeNewline() {
   const std::string code =
      "char c = '\n";

   const auto tokens = tokenizeString("invalid_unterminated_char_newline.djm", code);

   assertToken(tokens[0], TokenType::KwChar, "char", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "c", 1, 6);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 8);
   assertToken(tokens[3], TokenType::Invalid, "'", 1, 10);
}

static void testInvalidTooLongChar() {
   const std::string code =
      "char c = 'ab'\n";

   const auto tokens = tokenizeString("invalid_too_long_char.djm", code);

   assertToken(tokens[0], TokenType::KwChar, "char", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "c", 1, 6);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 8);
   assertToken(tokens[3], TokenType::Invalid, "'a", 1, 10);
   assertToken(tokens[4], TokenType::Identifier, "b", 1, 12);
   assertToken(tokens[5], TokenType::Invalid, "'", 1, 13);
   assertToken(tokens[6], TokenType::Newline, "\\n", 1, 14);

   assertToken(tokens[7], TokenType::EndOfFile, "", 2, 1);
}
