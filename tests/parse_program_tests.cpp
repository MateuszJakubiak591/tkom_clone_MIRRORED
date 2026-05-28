#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "source/StringSource.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

#include "syntax/Declarations.hpp"
#include "syntax/Type.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Expressions.hpp"

namespace {

ProgramPtr parseProgramFromString(const std::string& code) {
   StringSource source("test.djm", code);
   Lexer lexer(source);
   Parser parser(lexer);

   return parser.parseProgram();
}

struct ParseResult {
   ProgramPtr program;
   ErrorHandler errorHandler;
};

ParseResult parseProgramWithErrors(const std::string& code) {
   StringSource source("test.djm", code);
   Lexer lexer(source);

   ParseResult result{
      nullptr,
      ErrorHandler(code)
   };

   Parser parser(lexer, &result.errorHandler);
   result.program = parser.parseProgram();

   return result;
}

const FunctionDeclaration* asFunction(const DeclPtr& declaration) {
   return dynamic_cast<const FunctionDeclaration*>(declaration.get());
}

const GlobalConstantDeclaration* asGlobalConstant(const DeclPtr& declaration) {
   return dynamic_cast<const GlobalConstantDeclaration*>(declaration.get());
}

const ReturnStatement* asReturnStatement(const StmtPtr& statement) {
   return dynamic_cast<const ReturnStatement*>(statement.get());
}


}

TEST(ParserProgramTests, ParsesEmptyProgram) {
   auto program = parseProgramFromString("");

   ASSERT_NE(program, nullptr);
   EXPECT_TRUE(program->imports().empty());
   EXPECT_TRUE(program->declarations().empty());
}

TEST(ParserProgramTests, ParsesImportAll) {
   auto program = parseProgramFromString(
      "import * from \"utils.djm\"\n"
   );

   ASSERT_NE(program, nullptr);

   ASSERT_EQ(program->imports().size(), 1);
   EXPECT_TRUE(program->declarations().empty());

   const ImportDeclaration& import = *program->imports()[0];

   EXPECT_TRUE(import.importAll());
   EXPECT_TRUE(import.importedNames().empty());
   EXPECT_EQ(import.path(), "utils.djm");
}

TEST(ParserProgramTests, ParsesNamedImport) {
   auto program = parseProgramFromString(
      "import User, helper, MAX from \"file1.djm\"\n"
   );

   ASSERT_NE(program, nullptr);

   ASSERT_EQ(program->imports().size(), 1);
   EXPECT_TRUE(program->declarations().empty());

   const ImportDeclaration& import = *program->imports()[0];

   EXPECT_FALSE(import.importAll());

   ASSERT_EQ(import.importedNames().size(), 3);
   EXPECT_EQ(import.importedNames()[0], "User");
   EXPECT_EQ(import.importedNames()[1], "helper");
   EXPECT_EQ(import.importedNames()[2], "MAX");

   EXPECT_EQ(import.path(), "file1.djm");
}

TEST(ParserProgramTests, ParsesFunctionWithoutParameters) {
   auto program = parseProgramFromString(
      "fun answer() -> int {\n"
      "   return 36\n"
      "}\n"
   );

   ASSERT_NE(program, nullptr);

   EXPECT_TRUE(program->imports().empty());

   ASSERT_EQ(program->declarations().size(), 1);

   const FunctionDeclaration* function = asFunction(program->declarations()[0]);
   ASSERT_NE(function, nullptr);

   EXPECT_EQ(function->name(), "answer");
   EXPECT_TRUE(function->parameters().empty());

   const auto* returnType = dynamic_cast<const IntTypeNode*>(&function->returnType());
   ASSERT_NE(returnType, nullptr);

   ASSERT_EQ(function->body().statements().size(), 1);

   const ReturnStatement* returnStatement =
      asReturnStatement(function->body().statements()[0]);

   ASSERT_NE(returnStatement, nullptr);
   ASSERT_NE(returnStatement->expression(), nullptr);

   const auto* literal =
      dynamic_cast<const IntLiteralExpression*>(returnStatement->expression());

   ASSERT_NE(literal, nullptr);
   EXPECT_EQ(literal->value(), 36);
}

TEST(ParserProgramTests, ParsesFunctionWithParameters) {
   auto program = parseProgramFromString(
      "fun add(x: int, y: int) -> int {\n"
      "   return x + y\n"
      "}\n"
   );

   ASSERT_NE(program, nullptr);

   EXPECT_TRUE(program->imports().empty());

   ASSERT_EQ(program->declarations().size(), 1);

   const FunctionDeclaration* function = asFunction(program->declarations()[0]);
   ASSERT_NE(function, nullptr);

   EXPECT_EQ(function->name(), "add");

   ASSERT_EQ(function->parameters().size(), 2);

   EXPECT_EQ(function->parameters()[0].name, "x");
   EXPECT_NE(dynamic_cast<const IntTypeNode*>(function->parameters()[0].type.get()), nullptr);

   EXPECT_EQ(function->parameters()[1].name, "y");
   EXPECT_NE(dynamic_cast<const IntTypeNode*>(function->parameters()[1].type.get()), nullptr);

   const auto* returnType = dynamic_cast<const IntTypeNode*>(&function->returnType());
   ASSERT_NE(returnType, nullptr);

   ASSERT_EQ(function->body().statements().size(), 1);

   const ReturnStatement* returnStatement =
      asReturnStatement(function->body().statements()[0]);

   ASSERT_NE(returnStatement, nullptr);
   ASSERT_NE(returnStatement->expression(), nullptr);

   const auto* addExpression =
      dynamic_cast<const AddExpression*>(returnStatement->expression());

   ASSERT_NE(addExpression, nullptr);

   const auto* left =
      dynamic_cast<const IdentifierExpression*>(&addExpression->left());

   const auto* right =
      dynamic_cast<const IdentifierExpression*>(&addExpression->right());

   ASSERT_NE(left, nullptr);
   ASSERT_NE(right, nullptr);

   EXPECT_EQ(left->name(), "x");
   EXPECT_EQ(right->name(), "y");
}

TEST(ParserProgramTests, ParsesImportsBeforeFunctions) {
   auto program = parseProgramFromString(
      "import * from \"math.djm\"\n"
      "import User, helper from \"users.djm\"\n"
      "\n"
      "fun main() -> int {\n"
      "   return 0\n"
      "}\n"
   );

   ASSERT_NE(program, nullptr);

   ASSERT_EQ(program->imports().size(), 2);
   ASSERT_EQ(program->declarations().size(), 1);

   const ImportDeclaration& firstImport = *program->imports()[0];
   EXPECT_TRUE(firstImport.importAll());
   EXPECT_EQ(firstImport.path(), "math.djm");

   const ImportDeclaration& secondImport = *program->imports()[1];
   EXPECT_FALSE(secondImport.importAll());

   ASSERT_EQ(secondImport.importedNames().size(), 2);
   EXPECT_EQ(secondImport.importedNames()[0], "User");
   EXPECT_EQ(secondImport.importedNames()[1], "helper");
   EXPECT_EQ(secondImport.path(), "users.djm");

   const FunctionDeclaration* function = asFunction(program->declarations()[0]);
   ASSERT_NE(function, nullptr);

   EXPECT_EQ(function->name(), "main");
}

TEST(ParserProgramErrorTests, ReportsErrorForImportWithoutFrom) {
   const std::string code =
      "import * \"utils.djm\"\n";

   /*
   Error [Parser] at program.djm:1:10: expected 'from' after import spec
   import * "utils.djm"
            ^
   */

   StringSource source("test.djm", code);
   Lexer lexer(source);
   ErrorHandler errorHandler(code);
   Parser parser(lexer, &errorHandler);

   EXPECT_THROW(
      {
         auto program = parser.parseProgram();
         (void)program;
      },
      std::exception
   );

   EXPECT_TRUE(errorHandler.hasErrors());
   EXPECT_GT(errorHandler.errorCount(), 0);
}

TEST(ParserProgramErrorTests, ReportsErrorForFunctionWithoutArrow) {
   const std::string code =
      "fun main() int {\n"
      "   return 0\n"
      "}\n";

   /*
   Error [Parser] at program.djm:1:12: expected '->' after function parameters
   fun main() int {
   */

   StringSource source("test.djm", code);
   Lexer lexer(source);
   ErrorHandler errorHandler(code);
   Parser parser(lexer, &errorHandler);

   EXPECT_THROW(
      {
         auto program = parser.parseProgram();
         (void)program;
      },
      std::exception
   );

   EXPECT_TRUE(errorHandler.hasErrors());
   EXPECT_GT(errorHandler.errorCount(), 0);
}

TEST(ParserProgramErrorTests, ReportsErrorForParameterWithoutColon) {
   const std::string code =
      "fun add(x int) -> int {\n"
      "   return x\n"
      "}\n";

   /*
   Error [Parser] at program.djm:1:11: expected ':' after parameter name
   fun add(x int) -> int {
   */

   StringSource source("test.djm", code);
   Lexer lexer(source);
   ErrorHandler errorHandler(code);
   Parser parser(lexer, &errorHandler);

   EXPECT_THROW(
      {
         auto program = parser.parseProgram();
         (void)program;
      },
      std::exception
   );

   EXPECT_TRUE(errorHandler.hasErrors());
   EXPECT_GT(errorHandler.errorCount(), 0);
}

TEST(ParserProgramErrorTests, ReportsErrorForClassFieldWithoutColon) {
   const std::string code =
      "class User {\n"
      "   name string\n"
      "}\n";

   /*
   Error [Parser] at program.djm:2:9: expected '(' or ':' after identifier in class member
      name string
         ^
   */

   StringSource source("test.djm", code);
   Lexer lexer(source);
   ErrorHandler errorHandler(code);
   Parser parser(lexer, &errorHandler);

   EXPECT_THROW(
      {
         auto program = parser.parseProgram();
         (void)program;
      },
      std::exception
   );

   EXPECT_TRUE(errorHandler.hasErrors());
   EXPECT_GT(errorHandler.errorCount(), 0);
}

TEST(ParserProgramErrorTests, ReportsErrorForConstructorWithWrongName) {
   const std::string code =
      "class User {\n"
      "   Person(name: string) {\n"
      "      this.name = name\n"
      "   }\n"
      "}\n";
   
   /*
   Error [Parser] at program.djm:2:8: expected 'fun'
      mut setName(name: string) -> void {
   */

   StringSource source("test.djm", code);
   Lexer lexer(source);
   ErrorHandler errorHandler(code);
   Parser parser(lexer, &errorHandler);

   EXPECT_THROW(
      {
         auto program = parser.parseProgram();
         (void)program;
      },
      std::exception
   );

   EXPECT_TRUE(errorHandler.hasErrors());
   EXPECT_GT(errorHandler.errorCount(), 0);
}