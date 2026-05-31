#include "TestsUtils.hpp"

#include "lexer/Lexer.hpp"
#include "source/StringSource.hpp"
#include "source/FileSource.hpp"

#include <gtest/gtest.h>
#include <iostream>

void runTest(const std::string& name, void (*testFunction)()) {
   std::cout << "[ RUN      ] " << name << "\n";
   testFunction();
   std::cout << "[       OK ] " << name << "\n";
}

std::vector<Token> tokenizeString(
   const std::string& filename,
   const std::string& code
) {
   StringSource source(filename, code);
   Lexer lexer(source);

   std::vector<Token> tokens;

   while (true) {
      Token token = lexer.nextToken();
      tokens.push_back(token);

      if (token.type() == TokenType::EndOfFile) {
         break;
      }
   }

   return tokens;
}

std::vector<Token> tokenizeFile(const std::string& filename) {
   FileSource source(filename);
   Lexer lexer(source);

   std::vector<Token> tokens;

   while (true) {
      Token token = lexer.nextToken();
      tokens.push_back(token);

      if (token.type() == TokenType::EndOfFile) {
         break;
      }
   }

   return tokens;
}

void assertToken(
   const Token& token,
   TokenType expectedType,
   const std::string& expectedLexeme,
   int expectedLine,
   int expectedColumn
) {
   if (token.type() != expectedType ||
       token.lexeme() != expectedLexeme ||
       token.location().line != expectedLine ||
       token.location().column != expectedColumn) {

      std::cerr << "Expected: "
                << tokenTypeToString(expectedType)
                << " \"" << expectedLexeme << "\" at "
                << expectedLine << ":" << expectedColumn << "\n";

      std::cerr << "Actual:   "
                << tokenTypeToString(token.type())
                << " \"" << token.lexeme() << "\" at "
                << token.location().line << ":"
                << token.location().column << "\n";
   }

   EXPECT_EQ(token.type(), expectedType);
   EXPECT_EQ(token.lexeme(), expectedLexeme);
   EXPECT_EQ(token.location().line, expectedLine);
   EXPECT_EQ(token.location().column, expectedColumn);
}

void assertToken(
   const Token& token,
   TokenType expectedType,
   const std::string& expectedLexeme,
   const TokenValue& expectedValue
) {
   EXPECT_EQ(token.type(), expectedType);
   EXPECT_EQ(token.lexeme(), expectedLexeme);
   

   EXPECT_EQ(token.value(), expectedValue);
}

void assertNoInvalidTokens(const std::vector<Token>& tokens) {
   for (const Token& token : tokens) {
      if (token.type() == TokenType::Invalid) {
         std::cerr << "Unexpected invalid token: "
                   << "\"" << token.lexeme() << "\" at "
                   << token.location().line << ":"
                   << token.location().column << "\n";
      }

      assert(token.type() != TokenType::Invalid);
   }
}

// Nie jest to najbardziej eleganckie rozwiązanie, bo testowanie pojedyncznych
// statementów jest zależne od tego, czy działa parsowanie funkcji, ale tak by
// należało postąpić, jeśli parseStatement miałoby być prywatne
ProgramPtr parseString(
   const std::string& statementCode,
   ErrorHandler* errHandler
) {
   std::string code = "fun main() -> void {\n";
   code += statementCode;

   if (code.empty() || code.back() != '\n') {
      code += '\n';
   }

   code += "}\n";

   StringSource source("test.djm", code);
   Lexer lexer(source);
   Parser parser(lexer, errHandler);

   return parser.parseProgram();
}

Statement* firstStatement(Program* program) {
   if (program == nullptr) {
      return nullptr;
   }

   if (program->functionDeclarations().empty()) {
      return nullptr;
   }

   auto* function = program->functionDeclarations()[0].get();

   const auto& statements = function->body().statements();

   if (statements.empty()) {
      return nullptr;
   }

   return statements[0].get();
}
