#include <gtest/gtest.h>
#include "TestsUtils.hpp"

#include "lexer/Token.hpp"

#include <fstream>
#include <string>

TEST(FileSource, FileSourceSimpleDeclaration) {
   const std::string filename = "lexer_file_source_test.djm";

   {
      std::ofstream file(filename);
      file << "int x = 5\n";
   }

   const auto tokens = tokenizeFile(filename);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "x", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 7);
   assertToken(tokens[3], TokenType::IntLiteral, "5", 1, 9);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 10);
   assertToken(tokens[5], TokenType::EndOfFile, "", 2, 1);
}

static void testFileSourceWithFunction() {
   const std::string filename = "lexer_file_function_test.djm";

   {
      std::ofstream file(filename);
      file << "fun add(x: int, y: int) -> int {\n";
      file << "return x + y\n";
      file << "}\n";
   }

   const auto tokens = tokenizeFile(filename);

   assertToken(tokens[0], TokenType::KwFun, "fun", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "add", 1, 5);
   assertToken(tokens[2], TokenType::LParen, "(", 1, 8);
   assertToken(tokens[3], TokenType::Identifier, "x", 1, 9);
   assertToken(tokens[4], TokenType::Colon, ":", 1, 10);
   assertToken(tokens[5], TokenType::KwInt, "int", 1, 12);
   assertToken(tokens[6], TokenType::Comma, ",", 1, 15);
   assertToken(tokens[7], TokenType::Identifier, "y", 1, 17);
   assertToken(tokens[8], TokenType::Colon, ":", 1, 18);
   assertToken(tokens[9], TokenType::KwInt, "int", 1, 20);
   assertToken(tokens[10], TokenType::RParen, ")", 1, 23);
   assertToken(tokens[11], TokenType::Arrow, "->", 1, 25);
   assertToken(tokens[12], TokenType::KwInt, "int", 1, 28);
   assertToken(tokens[13], TokenType::LBrace, "{", 1, 32);
   assertToken(tokens[14], TokenType::Newline, "\\n", 1, 33);

   assertToken(tokens[15], TokenType::KwReturn, "return", 2, 1);
   assertToken(tokens[16], TokenType::Identifier, "x", 2, 8);
   assertToken(tokens[17], TokenType::Plus, "+", 2, 10);
   assertToken(tokens[18], TokenType::Identifier, "y", 2, 12);
   assertToken(tokens[19], TokenType::Newline, "\\n", 2, 13);

   assertToken(tokens[20], TokenType::RBrace, "}", 3, 1);
   assertToken(tokens[21], TokenType::Newline, "\\n", 3, 2);
   assertToken(tokens[22], TokenType::EndOfFile, "", 4, 1);
}
