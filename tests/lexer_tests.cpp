#include "lexer/Lexer.hpp"
#include "source/StringSource.hpp"
#include "source/FileSource.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static void runTest(const std::string& name, void (*testFunction)()) {
   std::cout << "[ RUN      ] " << name << "\n";
   testFunction();
   std::cout << "[       OK ] " << name << "\n";
}


static std::vector<Token> tokenizeString(const std::string& filename, const std::string& code) {
   StringSource source(filename, code);
   Lexer lexer(source);

   std::vector<Token> tokens;

   while (true) {
      Token token = lexer.nextToken();
      tokens.push_back(token);

      if (token.type == TokenType::EndOfFile) {
         break;
      }
   }

   return tokens;
}


static std::vector<Token> tokenizeFile(const std::string& filename) {
   FileSource source(filename);
   Lexer lexer(source);

   std::vector<Token> tokens;

   while (true) {
      Token token = lexer.nextToken();
      tokens.push_back(token);

      if (token.type == TokenType::EndOfFile) {
         break;
      }
   }

   return tokens;
}


static void assertToken(
   const Token& token,
   TokenType expectedType,
   const std::string& expectedLexeme,
   int expectedLine,
   int expectedColumn
) {
   if (token.type != expectedType ||
       token.lexeme != expectedLexeme ||
       token.location.line != expectedLine ||
       token.location.column != expectedColumn) {

      std::cerr << "Expected: "
                << tokenTypeToString(expectedType)
                << " \"" << expectedLexeme << "\" at "
                << expectedLine << ":" << expectedColumn << "\n";

      std::cerr << "Actual:   "
                << tokenTypeToString(token.type)
                << " \"" << token.lexeme << "\" at "
                << token.location.line << ":"
                << token.location.column << "\n";
   }

   assert(token.type == expectedType);
   assert(token.lexeme == expectedLexeme);
   assert(token.location.line == expectedLine);
   assert(token.location.column == expectedColumn);
}

static void testSimpleDeclaration() {
   const auto tokens = tokenizeString("test.djm", "int x = 5\n");

   assert(tokens.size() == 6);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "x", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 7);
   assertToken(tokens[3], TokenType::IntLiteral, "5", 1, 9);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 10);
   assertToken(tokens[5], TokenType::EndOfFile, "", 2, 1);
}

static void testKeywordsAndTypes() {
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
}

static void testOperators() {
   const std::string code =
      "x == y\n"
      "x != y\n"
      "x <= y\n"
      "x >= y\n"
      "!flag\n"
      "a && b || c\n"
      "xs contains ys\n"
      "xs |> this.name\n"
      "xs ? this.isActive\n"
      "xs % this.country\n";

   const auto tokens = tokenizeString("operators.djm", code);

   assertToken(tokens[0], TokenType::Identifier, "x", 1, 1);
   assertToken(tokens[1], TokenType::Equal, "==", 1, 3);
   assertToken(tokens[2], TokenType::Identifier, "y", 1, 6);

   assertToken(tokens[4], TokenType::Identifier, "x", 2, 1);
   assertToken(tokens[5], TokenType::NotEqual, "!=", 2, 3);
   assertToken(tokens[6], TokenType::Identifier, "y", 2, 6);

   assertToken(tokens[8], TokenType::Identifier, "x", 3, 1);
   assertToken(tokens[9], TokenType::LessEqual, "<=", 3, 3);
   assertToken(tokens[10], TokenType::Identifier, "y", 3, 6);

   assertToken(tokens[12], TokenType::Identifier, "x", 4, 1);
   assertToken(tokens[13], TokenType::GreaterEqual, ">=", 4, 3);
   assertToken(tokens[14], TokenType::Identifier, "y", 4, 6);

   assertToken(tokens[16], TokenType::Not, "!", 5, 1);
   assertToken(tokens[17], TokenType::Identifier, "flag", 5, 2);
}

static void testNumbersWithUnderscores() {
   const std::string code =
      "int a = 1_000\n"
      "float b = 1_000.25_50\n";

   const auto tokens = tokenizeString("numbers.djm", code);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "a", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 7);
   assertToken(tokens[3], TokenType::IntLiteral, "1_000", 1, 9);

   assertToken(tokens[5], TokenType::KwFloat, "float", 2, 1);
   assertToken(tokens[6], TokenType::Identifier, "b", 2, 7);
   assertToken(tokens[7], TokenType::Assign, "=", 2, 9);
   assertToken(tokens[8], TokenType::FloatLiteral, "1_000.25_50", 2, 11);
}

static void testListLiteralTokens() {
   const std::string code =
      "list<int> xs = [\n"
      "1,\n"
      "2,\n"
      "3\n"
      "]\n";

   const auto tokens = tokenizeString("list.djm", code);

   assertToken(tokens[0], TokenType::KwList, "list", 1, 1);
   assertToken(tokens[1], TokenType::Less, "<", 1, 5);
   assertToken(tokens[2], TokenType::KwInt, "int", 1, 6);
   assertToken(tokens[3], TokenType::Greater, ">", 1, 9);
   assertToken(tokens[4], TokenType::Identifier, "xs", 1, 11);
   assertToken(tokens[5], TokenType::Assign, "=", 1, 14);
   assertToken(tokens[6], TokenType::LBracket, "[", 1, 16);

   assertToken(tokens[7], TokenType::IntLiteral, "1", 2, 1);
   assertToken(tokens[8], TokenType::Comma, ",", 2, 2);
   assertToken(tokens[9], TokenType::IntLiteral, "2", 3, 1);
   assertToken(tokens[10], TokenType::Comma, ",", 3, 2);
   assertToken(tokens[11], TokenType::IntLiteral, "3", 4, 1);
   assertToken(tokens[12], TokenType::RBracket, "]", 5, 1);
}

static void testComments() {
   const std::string code =
      "int x = 5 // komentarz\n"
      "/* komentarz\n"
      "   wieloliniowy */\n"
      "int y = 7\n";

   const auto tokens = tokenizeString("comments.djm", code);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "x", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 7);
   assertToken(tokens[3], TokenType::IntLiteral, "5", 1, 9);
   assertToken(tokens[4], TokenType::Newline, "\\n", 1, 23);

   assertToken(tokens[5], TokenType::Newline, "\\n", 3, 19);
   assertToken(tokens[6], TokenType::KwInt, "int", 4, 1);
   assertToken(tokens[7], TokenType::Identifier, "y", 4, 5);
   assertToken(tokens[8], TokenType::Assign, "=", 4, 7);
   assertToken(tokens[9], TokenType::IntLiteral, "7", 4, 9);
}

static void testNewlineIgnoredInsideParentheses() {
   const std::string code =
      "int x = (5\n"
      "+ 7\n"
      "+ 3)\n";

   const auto tokens = tokenizeString("parentheses.djm", code);

   assertToken(tokens[0], TokenType::KwInt, "int", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "x", 1, 5);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 7);
   assertToken(tokens[3], TokenType::LParen, "(", 1, 9);
   assertToken(tokens[4], TokenType::IntLiteral, "5", 1, 10);

   // Wewnątrz nawiasów () lexer ignoruje newline
   assertToken(tokens[5], TokenType::Plus, "+", 2, 1);
   assertToken(tokens[6], TokenType::IntLiteral, "7", 2, 3);

   assertToken(tokens[7], TokenType::Plus, "+", 3, 1);
   assertToken(tokens[8], TokenType::IntLiteral, "3", 3, 3);
   assertToken(tokens[9], TokenType::RParen, ")", 3, 4);

   // Poza nawiasami () newline jest już normalnie tokenem
   assertToken(tokens[10], TokenType::Newline, "\\n", 3, 5);
   assertToken(tokens[11], TokenType::EndOfFile, "", 4, 1);
}

static void testStringsAndChars() {
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

static void testFileSource() {
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

int main() {
   runTest("SimpleDeclaration", testSimpleDeclaration);
   runTest("KeywordsAndTypes", testKeywordsAndTypes);
   runTest("Operators", testOperators);
   runTest("NumbersWithUnderscores", testNumbersWithUnderscores);
   runTest("ListLiteralTokens", testListLiteralTokens);
   runTest("Comments", testComments);
   runTest("NewlineIgnoredInsideParentheses", testNewlineIgnoredInsideParentheses);
   runTest("StringsAndChars", testStringsAndChars);
   runTest("FileSource", testFileSource);

   std::cout << "All lexer tests passed.\n";
   return 0;
}