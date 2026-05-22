#include <gtest/gtest.h>
#include "lexer/Token.hpp"
#include <stdexcept>
#include <string>

// Pomocnicza pusta lokacja dla testów
const SourceLocation loc{"file1", 1, 1};

TEST(TokenConsistency, IntLiteralWithWrongValueTypeThrows) {
   EXPECT_THROW(
      Token(TokenType::IntLiteral, "123", loc, std::string("123")), 
      std::invalid_argument
   );

   EXPECT_THROW(
      Token(TokenType::IntLiteral, "123", loc, 123.0), 
      std::invalid_argument
   );
}

TEST(TokenConsistency, FloatLiteralWithWrongValueTypeThrows) {
   EXPECT_THROW(
      Token(TokenType::FloatLiteral, "3.14", loc, int64_t(3)), 
      std::invalid_argument
   );

   EXPECT_THROW(
      Token(TokenType::FloatLiteral, "3.14", loc, std::string("3.14")), 
      std::invalid_argument
   );
}

TEST(TokenConsistency, StringAndCharLiteralsWithWrongValueTypeThrows) {
   EXPECT_THROW(
      Token(TokenType::StringLiteral, "\"abc\"", loc, int64_t(123)), 
      std::invalid_argument
   );

   EXPECT_THROW(
      Token(TokenType::CharLiteral, "'a'", loc, std::string("a")), 
      std::invalid_argument
   );
}

TEST(TokenConsistency, BoolLiteralChecksBothTypeAndLexeme) {
   EXPECT_THROW(
      Token(TokenType::BoolLiteral, "true", loc, int64_t(1)), 
      std::invalid_argument
   );

   EXPECT_THROW(
      Token(TokenType::BoolLiteral, "True", loc, true), // Powinno być "true" z małej litery
      std::invalid_argument
   );
   EXPECT_THROW(
      Token(TokenType::BoolLiteral, "1", loc, true),
      std::invalid_argument
   );
}

TEST(TokenConsistency, KeywordsAndIdentifiersMustNotHaveValues) {
   // Słowa kluczowe i identyfikatory powinny mieć jako value monostate
   EXPECT_THROW(
      Token(TokenType::Identifier, "myVariable", loc, std::string("myVariable")), 
      std::invalid_argument
   );

   EXPECT_THROW(
      Token(TokenType::KwInt, "int", loc, int64_t(0)), 
      std::invalid_argument
   );
}

TEST(TokenConsistency, FactoryFunctionsEnsureConsistency) {
   EXPECT_NO_THROW(makeIntToken("123", loc, 123));
   EXPECT_NO_THROW(makeFloatToken("3.14", loc, 3.14));
   EXPECT_NO_THROW(makeStringToken("\"hello\"", loc, "hello"));
   EXPECT_NO_THROW(makeBoolToken("true", loc, true));
   EXPECT_NO_THROW(makeToken(TokenType::KwIf, "if", loc)); // Domyślnie monostate
}