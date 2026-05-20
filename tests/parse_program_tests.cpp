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

std::unique_ptr<Program> parseProgramFromString(const std::string& code) {
   StringSource source("test.djm", code);
   Lexer lexer(source);
   Parser parser(lexer);

   return parser.parseProgram();
}

struct ParseResult {
   std::unique_ptr<Program> program;
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

const ClassDeclaration* asClass(const DeclPtr& declaration) {
   return dynamic_cast<const ClassDeclaration*>(declaration.get());
}

const GlobalConstantDeclaration* asGlobalConstant(const DeclPtr& declaration) {
   return dynamic_cast<const GlobalConstantDeclaration*>(declaration.get());
}

const FieldDeclaration* asField(const ClassMemberPtr& member) {
   return dynamic_cast<const FieldDeclaration*>(member.get());
}

const MethodDeclaration* asMethod(const ClassMemberPtr& member) {
   return dynamic_cast<const MethodDeclaration*>(member.get());
}

const ConstructorDeclaration* asConstructor(const ClassMemberPtr& member) {
   return dynamic_cast<const ConstructorDeclaration*>(member.get());
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

TEST(ParserProgramTests, ParsesClassWithField) {
   auto result = parseProgramWithErrors(
      "class User {\n"
      "   name: string\n"
      "}\n"
   );

   ASSERT_FALSE(result.errorHandler.hasErrors());
   ASSERT_NE(result.program, nullptr);

   ASSERT_EQ(result.program->declarations().size(), 1);

   const ClassDeclaration* cls =
      asClass(result.program->declarations()[0]);

   ASSERT_NE(cls, nullptr);
   EXPECT_EQ(cls->name(), "User");

   ASSERT_EQ(cls->members().size(), 1);

   const FieldDeclaration* field = asField(cls->members()[0]);
   ASSERT_NE(field, nullptr);

   EXPECT_EQ(field->modifier(), FieldModifier::None);
   EXPECT_EQ(field->name(), "name");
   EXPECT_NE(dynamic_cast<const StringTypeNode*>(&field->type()), nullptr);
   EXPECT_EQ(field->initializer(), nullptr);
}

TEST(ParserProgramTests, ParsesClassWithPrivateAndStaticFields) {
   // Poniżej count1: int ponieważ count jest słowem kluczowym .djm
   auto result = parseProgramWithErrors(
      "class User {\n"
      "   private name: string\n"
      "   static count1: int = 0\n"
      "}\n"
   );

   ASSERT_FALSE(result.errorHandler.hasErrors());
   ASSERT_NE(result.program, nullptr);

   ASSERT_EQ(result.program->declarations().size(), 1);

   const ClassDeclaration* cls =
      asClass(result.program->declarations()[0]);

   ASSERT_NE(cls, nullptr);
   ASSERT_EQ(cls->members().size(), 2);

   const FieldDeclaration* privateField = asField(cls->members()[0]);
   ASSERT_NE(privateField, nullptr);
   EXPECT_EQ(privateField->modifier(), FieldModifier::Private);

   const FieldDeclaration* staticField = asField(cls->members()[1]);
   ASSERT_NE(staticField, nullptr);
   EXPECT_EQ(staticField->modifier(), FieldModifier::Static);
   ASSERT_NE(staticField->initializer(), nullptr);

   const auto* initializer =
      dynamic_cast<const IntLiteralExpression*>(staticField->initializer());

   ASSERT_NE(initializer, nullptr);
   EXPECT_EQ(initializer->value(), 0);
}

TEST(ParserProgramTests, ParsesClassWithConstructor) {
   auto result = parseProgramWithErrors(
      "class User {\n"
      "   User(name: string) {\n"
      "      this.name = name\n"
      "   }\n"
      "}\n"
   );

   ASSERT_FALSE(result.errorHandler.hasErrors());
   ASSERT_NE(result.program, nullptr);

   ASSERT_EQ(result.program->declarations().size(), 1);

   const ClassDeclaration* cls =
      asClass(result.program->declarations()[0]);

   ASSERT_NE(cls, nullptr);
   ASSERT_EQ(cls->members().size(), 1);

   const ConstructorDeclaration* constructor =
      asConstructor(cls->members()[0]);

   ASSERT_NE(constructor, nullptr);

   EXPECT_EQ(constructor->name(), "User");
   ASSERT_EQ(constructor->parameters().size(), 1);
   EXPECT_EQ(constructor->parameters()[0].name, "name");

   ASSERT_EQ(constructor->body().statements().size(), 1);

   const auto* assignment =
      dynamic_cast<const AssignmentStatement*>(
         constructor->body().statements()[0].get()
      );

   ASSERT_NE(assignment, nullptr);
}

TEST(ParserProgramTests, ParsesClassWithMethod) {
   auto result = parseProgramWithErrors(
      "class User {\n"
      "   fun getName() -> string {\n"
      "      return this.name\n"
      "   }\n"
      "}\n"
   );

   ASSERT_FALSE(result.errorHandler.hasErrors());
   ASSERT_NE(result.program, nullptr);

   ASSERT_EQ(result.program->declarations().size(), 1);

   const ClassDeclaration* cls =
      asClass(result.program->declarations()[0]);

   ASSERT_NE(cls, nullptr);
   ASSERT_EQ(cls->members().size(), 1);

   const MethodDeclaration* method = asMethod(cls->members()[0]);
   ASSERT_NE(method, nullptr);

   EXPECT_EQ(method->modifier(), MethodModifier::None);
   EXPECT_EQ(method->function().name(), "getName");
   EXPECT_NE(
      dynamic_cast<const StringTypeNode*>(&method->function().returnType()),
      nullptr
   );

   ASSERT_EQ(method->function().body().statements().size(), 1);
   EXPECT_NE(
      dynamic_cast<const ReturnStatement*>(
         method->function().body().statements()[0].get()
      ),
      nullptr
   );
}

TEST(ParserProgramTests, ParsesClassWithMutAndStaticMethods) {
   auto result = parseProgramWithErrors(
      "class Counter {\n"
      "   mut fun increment() -> void {\n"
      "      this.value = this.value + 1\n"
      "   }\n"
      "\n"
      "   static fun zero() -> int {\n"
      "      return 0\n"
      "   }\n"
      "}\n"
   );

   ASSERT_FALSE(result.errorHandler.hasErrors());
   ASSERT_NE(result.program, nullptr);

   ASSERT_EQ(result.program->declarations().size(), 1);

   const ClassDeclaration* cls =
      asClass(result.program->declarations()[0]);

   ASSERT_NE(cls, nullptr);
   ASSERT_EQ(cls->members().size(), 2);

   const MethodDeclaration* mutMethod = asMethod(cls->members()[0]);
   ASSERT_NE(mutMethod, nullptr);
   EXPECT_EQ(mutMethod->modifier(), MethodModifier::Mut);
   EXPECT_EQ(mutMethod->function().name(), "increment");

   const MethodDeclaration* staticMethod = asMethod(cls->members()[1]);
   ASSERT_NE(staticMethod, nullptr);
   EXPECT_EQ(staticMethod->modifier(), MethodModifier::Static);
   EXPECT_EQ(staticMethod->function().name(), "zero");
}

TEST(ParserProgramTests, ParsesMixedProgramWithImportsFunctionAndClass) {
   auto result = parseProgramWithErrors(
      "import * from \"base.djm\"\n"
      "\n"
      "fun main() -> int {\n"
      "   return 0\n"
      "}\n"
      "\n"
      "class User {\n"
      "   name: string\n"
      "}\n"
   );

   ASSERT_FALSE(result.errorHandler.hasErrors());
   ASSERT_NE(result.program, nullptr);

   ASSERT_EQ(result.program->imports().size(), 1);
   ASSERT_EQ(result.program->declarations().size(), 2);

   EXPECT_NE(asFunction(result.program->declarations()[0]), nullptr);
   EXPECT_NE(asClass(result.program->declarations()[1]), nullptr);
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