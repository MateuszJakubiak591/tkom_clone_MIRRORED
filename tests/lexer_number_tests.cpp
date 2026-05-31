#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"

#include <iostream>
#include <string>

namespace {
template <typename T>
void assertSingleLiteral(
   const std::string& source,
   TokenType expectedType,
   const T& expectedValue
) {
   const auto tokens = tokenizeString("single_number.djm", source);

   ASSERT_EQ(tokens.size(), 2);
   assertToken(tokens[0], expectedType, source, 1, 1);
   EXPECT_EQ(std::get<T>(tokens[0].value()), expectedValue);
   assertToken(tokens[1], TokenType::EndOfFile, "", 1, static_cast<int>(source.size()) + 1);
}
}

TEST(LexerNumbers, IntegerLiterals) {
   const std::string code =
      "int a = 0\n"
      "int b = 123\n"
      "int c = 1_000\n"
      "int d = 2_147_483_647\n";

   const auto tokens = tokenizeString("integer_literals.djm", code);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "a", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 7);
   assertToken(tokens[3], TokenType::IntLiteral, "0", 1, 9);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 10);

   assertToken(tokens[5], TokenType::KwInt, "int", 2, 1);
   assertToken(tokens[6], TokenType::Identifier, "b", 2, 5);
   assertToken(tokens[7], TokenType::Assign, "=", 2, 7);
   assertToken(tokens[8], TokenType::IntLiteral, "123", 2, 9);
   EXPECT_EQ(std::get<int64_t>(tokens[8].value()), 123);
   assertToken(tokens[9], TokenType::Newline, "\\n", 2, 12);

   assertToken(tokens[10], TokenType::KwInt, "int", 3, 1);
   assertToken(tokens[11], TokenType::Identifier, "c", 3, 5);
   assertToken(tokens[12], TokenType::Assign, "=", 3, 7);
   assertToken(tokens[13], TokenType::IntLiteral, "1_000", 3, 9);
   assertToken(tokens[14], TokenType::Newline, "\\n", 3, 14);

   assertToken(tokens[15], TokenType::KwInt, "int", 4, 1);
   assertToken(tokens[16], TokenType::Identifier, "d", 4, 5);
   assertToken(tokens[17], TokenType::Assign, "=", 4, 7);
   assertToken(tokens[18], TokenType::IntLiteral, "2_147_483_647", 4, 9);
   EXPECT_EQ(std::get<int64_t>(tokens[18].value()), 2147483647);
   assertToken(tokens[19], TokenType::Newline, "\\n", 4, 22);

   assertToken(tokens[20], TokenType::EndOfFile, "", 5, 1);
}

TEST(LexerNumbers, FloatLiterals) {
   const std::string code =
      "float a = 0.0\n"
      "float b = 123.456\n"
      "float c = 1_000.25\n"
      "float d = 3.141_592\n";

   const auto tokens = tokenizeString("float_literals.djm", code);

   assertToken(tokens[0], TokenType::KwFloat, "float", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "a", 1, 7);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 9);
   assertToken(tokens[3], TokenType::FloatLiteral, "0.0", 1, 11);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 14);

   assertToken(tokens[5], TokenType::KwFloat, "float", 2, 1);
   assertToken(tokens[6], TokenType::Identifier, "b", 2, 7);
   assertToken(tokens[7], TokenType::Assign, "=", 2, 9);
   assertToken(tokens[8], TokenType::FloatLiteral, "123.456", 2, 11);
   EXPECT_EQ(std::get<double>(tokens[8].value()), 123.456);
   assertToken(tokens[9], TokenType::Newline, "\\n", 2, 18);

   assertToken(tokens[10], TokenType::KwFloat, "float", 3, 1);
   assertToken(tokens[11], TokenType::Identifier, "c", 3, 7);
   assertToken(tokens[12], TokenType::Assign, "=", 3, 9);
   assertToken(tokens[13], TokenType::FloatLiteral, "1_000.25", 3, 11);
   assertToken(tokens[14], TokenType::Newline, "\\n", 3, 19);

   assertToken(tokens[15], TokenType::KwFloat, "float", 4, 1);
   assertToken(tokens[16], TokenType::Identifier, "d", 4, 7);
   assertToken(tokens[17], TokenType::Assign, "=", 4, 9);
   assertToken(tokens[18], TokenType::FloatLiteral, "3.141_592", 4, 11);
   assertToken(tokens[19], TokenType::Newline, "\\n", 4, 20);

   assertToken(tokens[20], TokenType::EndOfFile, "", 5, 1);
}

TEST(LexerNumbers, InvalidNumberLiterals) {
   const std::string code =
      "int a = 1__000\n"
      "int b = 123_\n"
      "float c = 1_.5\n"
      "float d = 1.2__3\n"; 

   const auto tokens = tokenizeString("invalid_number_literals.djm", code);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "a", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 7);
   assertToken(tokens[3], TokenType::Invalid, "1__000", 1, 9);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 15);

   assertToken(tokens[5], TokenType::KwInt, "int", 2, 1);
   assertToken(tokens[6], TokenType::Identifier, "b", 2, 5);
   assertToken(tokens[7], TokenType::Assign, "=", 2, 7);
   assertToken(tokens[8], TokenType::Invalid, "123_", 2, 9);
   assertToken(tokens[9], TokenType::Newline, "\\n", 2, 13);

   assertToken(tokens[10], TokenType::KwFloat, "float", 3, 1);
   assertToken(tokens[11], TokenType::Identifier, "c", 3, 7);
   assertToken(tokens[12], TokenType::Assign, "=", 3, 9);
   assertToken(tokens[13], TokenType::Invalid, "1_.5", 3, 11);
   assertToken(tokens[14], TokenType::Newline, "\\n", 3, 15);

   assertToken(tokens[15], TokenType::KwFloat, "float", 4, 1);
   assertToken(tokens[16], TokenType::Identifier, "d", 4, 7);
   assertToken(tokens[17], TokenType::Assign, "=", 4, 9);
   assertToken(tokens[18], TokenType::Invalid, "1.2__3", 4, 11);
   assertToken(tokens[19], TokenType::Newline, "\\n", 4, 17);

   assertToken(tokens[20], TokenType::EndOfFile, "", 5, 1);
}

TEST(LexerNumbers, TokenizesIntegerLiteralsInIsolation) {
   assertSingleLiteral<int64_t>("0", TokenType::IntLiteral, 0);
   assertSingleLiteral<int64_t>("123", TokenType::IntLiteral, 123);
   assertSingleLiteral<int64_t>("1_000", TokenType::IntLiteral, 1000);
   assertSingleLiteral<int64_t>("2_147_483_647", TokenType::IntLiteral, 2147483647);
}

TEST(LexerNumbers, TokenizesFloatLiteralsInIsolation) {
   assertSingleLiteral<double>("0.0", TokenType::FloatLiteral, 0.0);
   assertSingleLiteral<double>("123.456", TokenType::FloatLiteral, 123.456);
   assertSingleLiteral<double>("1_000.25", TokenType::FloatLiteral, 1000.25);
   assertSingleLiteral<double>("3.141_592", TokenType::FloatLiteral, 3.141592);
}
