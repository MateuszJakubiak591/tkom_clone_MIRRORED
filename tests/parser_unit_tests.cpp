#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "lexer/TokenSource.hpp"
#include "lexer/Token.hpp"
#include "parser/Parser.hpp"
#include "diagnostics/ErrorHandler.hpp"

#include "syntax/Declarations.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Expressions.hpp"
#include "syntax/Type.hpp"

namespace {

SourceLocation loc(int line = 1, int column = 1) {
   return SourceLocation{"unit_test.djm", line, column};
}

class VectorTokenSource final : public TokenSource {
public:
   explicit VectorTokenSource(std::vector<Token> tokens)
      : tokens_(std::move(tokens)) {}

   Token nextToken() override {
      if (position_ < tokens_.size()) {
         return tokens_[position_++];
      }

      return makeToken(TokenType::EndOfFile, "", loc());
   }

private:
   std::vector<Token> tokens_;
   std::size_t position_ = 0;
};

std::unique_ptr<Program> parseProgramFromTokens(
   std::vector<Token> tokens,
   ErrorHandler& errorHandler
) {
   tokens.push_back(makeToken(TokenType::EndOfFile, "", loc()));

   VectorTokenSource tokenSource(std::move(tokens));
   Parser parser(tokenSource, &errorHandler);

   return parser.parseProgram();
}

const FunctionDeclaration* asFunction(const DeclPtr& declaration) {
   return dynamic_cast<const FunctionDeclaration*>(declaration.get());
}

}


// a + b * c
TEST(ParserUnitExpressionTests, ParsesAdditiveAndMultiplicativePrecedence) {
   ErrorHandler errorHandler;

   auto program = parseProgramFromTokens(
      {
         makeToken(TokenType::KwFun, "fun", loc(1, 1)),
         makeToken(TokenType::Identifier, "main", loc(1, 5)),
         makeToken(TokenType::LParen, "(", loc(1, 9)),
         makeToken(TokenType::RParen, ")", loc(1, 10)),
         makeToken(TokenType::Arrow, "->", loc(1, 12)),
         makeToken(TokenType::KwInt, "int", loc(1, 15)),
         makeToken(TokenType::LBrace, "{", loc(1, 19)),
         makeToken(TokenType::Newline, "\\n", loc(1, 20)),

         makeToken(TokenType::Identifier, "a", loc(1, 1)),
         makeToken(TokenType::Plus, "+", loc(1, 3)),
         makeToken(TokenType::Identifier, "b", loc(1, 5)),
         makeToken(TokenType::Multiply, "*", loc(1, 7)),
         makeToken(TokenType::Identifier, "c", loc(1, 9)),

         makeToken(TokenType::RBrace, "}", loc(3, 1)),
         makeToken(TokenType::Newline, "\\n", loc(3, 2)),
      },
      errorHandler
   );

   ASSERT_FALSE(errorHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   ASSERT_EQ(program->declarations().size(), 1);

   const FunctionDeclaration* function = asFunction(program->declarations()[0]);
   ASSERT_NE(function, nullptr);

   const auto& statements = function->body().statements();

   ASSERT_EQ(statements.size(), 1);

   const auto* exprStmt =
      dynamic_cast<const ExpressionStatement*>(
         statements[0].get()
      );

   const auto* add = dynamic_cast<const AddExpression*>(&exprStmt->expression());
   ASSERT_NE(add, nullptr);

   const auto* left = dynamic_cast<const IdentifierExpression*>(&add->left());
   ASSERT_NE(left, nullptr);
   EXPECT_EQ(left->name(), "a");

   const auto* multiply = dynamic_cast<const MultiplyExpression*>(&add->right());
   ASSERT_NE(multiply, nullptr);

   const auto* mulLeft =
      dynamic_cast<const IdentifierExpression*>(&multiply->left());

   const auto* mulRight =
      dynamic_cast<const IdentifierExpression*>(&multiply->right());

   ASSERT_NE(mulLeft, nullptr);
   ASSERT_NE(mulRight, nullptr);

   EXPECT_EQ(mulLeft->name(), "b");
   EXPECT_EQ(mulRight->name(), "c");
}

//user.getName()
TEST(ParserUnitProgramTests, ParsesMethodCall) {
   ErrorHandler errorHandler;

   auto program = parseProgramFromTokens(
      {
         makeToken(TokenType::KwFun, "fun", loc(1, 1)),
         makeToken(TokenType::Identifier, "main", loc(1, 5)),
         makeToken(TokenType::LParen, "(", loc(1, 9)),
         makeToken(TokenType::RParen, ")", loc(1, 10)),
         makeToken(TokenType::Arrow, "->", loc(1, 12)),
         makeToken(TokenType::KwInt, "int", loc(1, 15)),
         makeToken(TokenType::LBrace, "{", loc(1, 19)),
         makeToken(TokenType::Newline, "\\n", loc(1, 20)),

         makeToken(TokenType::Identifier, "user", loc(1, 1)),
         makeToken(TokenType::Dot, ".", loc(1, 5)),
         makeToken(TokenType::Identifier, "getName", loc(1, 6)),
         makeToken(TokenType::LParen, "(", loc(1, 13)),
         makeToken(TokenType::RParen, ")", loc(1, 14)),

         makeToken(TokenType::RBrace, "}", loc(3, 1)),
         makeToken(TokenType::Newline, "\\n", loc(3, 2)),
      },
      errorHandler
   );

   ASSERT_FALSE(errorHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   ASSERT_EQ(program->declarations().size(), 1);

   const FunctionDeclaration* function = asFunction(program->declarations()[0]);
   ASSERT_NE(function, nullptr);

   const auto& statements = function->body().statements();

   ASSERT_EQ(statements.size(), 1);

   const auto* exprStmt =
      dynamic_cast<const ExpressionStatement*>(
         statements[0].get()
      );

   ASSERT_NE(exprStmt, nullptr);

   const auto* call =
      dynamic_cast<const CallExpression*>(
         &exprStmt->expression()
      );

   const auto* member =
      dynamic_cast<const MemberAccessExpression*>(&call->callee());

   ASSERT_NE(member, nullptr);
   EXPECT_EQ(member->memberName(), "getName");

   const auto* object =
      dynamic_cast<const IdentifierExpression*>(&member->object());

   ASSERT_NE(object, nullptr);
   EXPECT_EQ(object->name(), "user");
}

// import * from "utils.djm"
TEST(ParserUnitProgramTests, ParsesImportAllFromTokens) {
   ErrorHandler errorHandler;

   auto program = parseProgramFromTokens(
      {
         makeToken(TokenType::KwImport, "import", loc(1, 1)),
         makeToken(TokenType::Multiply, "*", loc(1, 8)),
         makeToken(TokenType::KwFrom, "from", loc(1, 10)),
         makeStringToken("\"utils.djm\"", loc(1, 15), "utils.djm"),
         makeToken(TokenType::Newline, "\\n", loc(1, 26)),
      },
      errorHandler
   );

   ASSERT_FALSE(errorHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   ASSERT_EQ(program->imports().size(), 1);
   EXPECT_TRUE(program->declarations().empty());

   const ImportDeclaration& import = *program->imports()[0];

   EXPECT_TRUE(import.importAll());
   EXPECT_TRUE(import.importedNames().empty());
   EXPECT_EQ(import.path(), "utils.djm");
}

/*
fun main() -> int{
   return 0
}
*/

TEST(ParserUnitProgramTests, ParsesFunctionWithoutParametersFromTokens) {
   ErrorHandler errorHandler;

   auto program = parseProgramFromTokens(
      {
         makeToken(TokenType::KwFun, "fun", loc(1, 1)),
         makeToken(TokenType::Identifier, "main", loc(1, 5)),
         makeToken(TokenType::LParen, "(", loc(1, 9)),
         makeToken(TokenType::RParen, ")", loc(1, 10)),
         makeToken(TokenType::Arrow, "->", loc(1, 12)),
         makeToken(TokenType::KwInt, "int", loc(1, 15)),
         makeToken(TokenType::LBrace, "{", loc(1, 19)),
         makeToken(TokenType::Newline, "\\n", loc(1, 20)),

         makeToken(TokenType::KwReturn, "return", loc(2, 4)),
         makeIntToken("0", loc(2, 11), 0),
         makeToken(TokenType::Newline, "\\n", loc(2, 12)),

         makeToken(TokenType::RBrace, "}", loc(3, 1)),
         makeToken(TokenType::Newline, "\\n", loc(3, 2)),
      },
      errorHandler
   );

   ASSERT_FALSE(errorHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   ASSERT_EQ(program->declarations().size(), 1);

   const FunctionDeclaration* function = asFunction(program->declarations()[0]);
   ASSERT_NE(function, nullptr);

   EXPECT_EQ(function->name(), "main");
   EXPECT_TRUE(function->parameters().empty());
   EXPECT_NE(dynamic_cast<const IntTypeNode*>(&function->returnType()), nullptr);

   ASSERT_EQ(function->body().statements().size(), 1);

   const auto* ret =
      dynamic_cast<const ReturnStatement*>(
         function->body().statements()[0].get()
      );

   ASSERT_NE(ret, nullptr);
   ASSERT_NE(ret->expression(), nullptr);
}


/*
fun main() int {
}
*/
TEST(ParserUnitErrorTests, ReportsFunctionWithoutArrow) {
   ErrorHandler errorHandler;

   std::vector<Token> tokens = {
      makeToken(TokenType::KwFun, "fun", loc(1, 1)),
      makeToken(TokenType::Identifier, "main", loc(1, 5)),
      makeToken(TokenType::LParen, "(", loc(1, 9)),
      makeToken(TokenType::RParen, ")", loc(1, 10)),
      makeToken(TokenType::KwInt, "int", loc(1, 12)),
      makeToken(TokenType::LBrace, "{", loc(1, 16)),
      makeToken(TokenType::RBrace, "}", loc(1, 17)),
      makeToken(TokenType::EndOfFile, "", loc(1, 18)),
   };

   VectorTokenSource tokenSource(std::move(tokens));
   Parser parser(tokenSource, &errorHandler);

   EXPECT_THROW(
      {
         auto program = parser.parseProgram();
         (void)program;
      },
      std::exception
   );

   EXPECT_TRUE(errorHandler.hasErrors());
}
