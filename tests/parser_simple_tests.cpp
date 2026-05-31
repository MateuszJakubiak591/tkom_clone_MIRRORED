#include <gtest/gtest.h>
#include "TestsUtils.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Expressions.hpp"

TEST(ParserSimpleTests, ParseImmutableVariableWithoutInitializer) {
   std::string code = "string name\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt);

   ASSERT_NE(varDecl, nullptr);
   EXPECT_FALSE(varDecl->isMutable());
   EXPECT_EQ(varDecl->initializer(), nullptr);

   auto* stringType = dynamic_cast<const StringTypeNode*>(&varDecl->type());
   ASSERT_NE(stringType, nullptr);
   EXPECT_EQ(varDecl->names()[0].name, "name");
}

TEST(ParserSimpleTests, ParseVariableDeclarationWithListType) {
   std::string code = "list<int> numbers = [1, 2]\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt);
   ASSERT_NE(varDecl, nullptr);

   auto* listType = dynamic_cast<const ListTypeNode*>(&varDecl->type());
   ASSERT_NE(listType, nullptr);
   
   auto* elemType = dynamic_cast<const IntTypeNode*>(&listType->elementType());
   ASSERT_NE(elemType, nullptr);

   auto* listLit = dynamic_cast<const ListLiteralExpression*>(varDecl->initializer());
   ASSERT_NE(listLit, nullptr);
   EXPECT_EQ(listLit->elements().size(), 2);
}

TEST(ParserSimpleTests, ParseAssignmentStatement) {
   std::string code = "counter = 10\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* assignStmt = dynamic_cast<AssignmentStatement*>(stmt);
   ASSERT_NE(assignStmt, nullptr);

   auto* idExpr = dynamic_cast<const IdentifierExpression*>(&assignStmt->target());
   ASSERT_NE(idExpr, nullptr);
   EXPECT_EQ(idExpr->name(), "counter");

   auto* intLit = dynamic_cast<const IntLiteralExpression*>(&assignStmt->value());
   ASSERT_NE(intLit, nullptr);
   EXPECT_EQ(intLit->value(), 10);
}

TEST(ParserSimpleTests, ParseVariableDeclaration) {
   std::string code = "mut int x = 25\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt);
   ASSERT_NE(varDecl, nullptr);

   EXPECT_TRUE(varDecl->isMutable());

   auto* intType = dynamic_cast<const IntTypeNode*>(&varDecl->type());
   ASSERT_NE(intType, nullptr);

   ASSERT_EQ(varDecl->names().size(), 1);
   EXPECT_EQ(varDecl->names()[0].name, "x");

   ASSERT_NE(varDecl->initializer(), nullptr);
   auto* initExpr = dynamic_cast<const IntLiteralExpression*>(varDecl->initializer());
   ASSERT_NE(initExpr, nullptr);
   EXPECT_EQ(initExpr->value(), 25);
}

TEST(ParserSimpleTests, ParsePipelineMapExpression) {
   std::string code = "data |> transform\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);
   auto* mapExpr = dynamic_cast<const MapExpression*>(&exprStmt->expression());
   ASSERT_NE(mapExpr, nullptr);
   
   auto* left = dynamic_cast<const IdentifierExpression*>(&mapExpr->left());
   EXPECT_EQ(left->name(), "data");

   auto* right = dynamic_cast<const IdentifierExpression*>(&mapExpr->right());
   EXPECT_EQ(right->name(), "transform");
}

TEST(ParserSimpleTests, ParseSimpleAddExpression) {
   std::string code = "5 + x\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);
   ASSERT_NE(exprStmt, nullptr);

   auto* addExpr = dynamic_cast<const AddExpression*>(&exprStmt->expression());
   ASSERT_NE(addExpr, nullptr);

   auto* left = dynamic_cast<const IntLiteralExpression*>(&addExpr->left());
   ASSERT_NE(left, nullptr);
   EXPECT_EQ(left->value(), 5);

   auto* right = dynamic_cast<const IdentifierExpression*>(&addExpr->right());
   ASSERT_NE(right, nullptr);
   EXPECT_EQ(right->name(), "x");
}

TEST(ParserSimpleTests, ParseOperatorMulBeforeAdd) {
   std::string code = "2 + 3 * 4\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);
   auto* mainAdd = dynamic_cast<const AddExpression*>(&exprStmt->expression());
   ASSERT_NE(mainAdd, nullptr);

   auto* two = dynamic_cast<const IntLiteralExpression*>(&mainAdd->left());
   ASSERT_NE(two, nullptr);
   EXPECT_EQ(two->value(), 2);

   auto* mulExpr = dynamic_cast<const MultiplyExpression*>(&mainAdd->right());
   ASSERT_NE(mulExpr, nullptr);
}

TEST(ParserSimpleTests, ParseRightAssociativePowerExpression) {
   std::string code = "2 ^ 3 ^ 4\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);
   auto* mainPower = dynamic_cast<const PowerExpression*>(&exprStmt->expression());
   ASSERT_NE(mainPower, nullptr);

   auto* leftBase = dynamic_cast<const IntLiteralExpression*>(&mainPower->left());
   ASSERT_NE(leftBase, nullptr);
   EXPECT_EQ(leftBase->value(), 2);

   auto* nestedPower = dynamic_cast<const PowerExpression*>(&mainPower->right());
   ASSERT_NE(nestedPower, nullptr);
}

// Być może poniższe nie jest poprawne semantycznie, ale
// ukazuje własność gramatyki polegającą na tym, że można
// stosować wiele operatorów unarnych po kolei
TEST(ParserSimpleTests, ParseUnaryNotAndNegate) {
   std::string code = "!-true\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);
   auto* notExpr = dynamic_cast<const NotExpression*>(&exprStmt->expression());
   ASSERT_NE(notExpr, nullptr);

   auto* negateExpr = dynamic_cast<const NegateExpression*>(&notExpr->operand());
   ASSERT_NE(negateExpr, nullptr);

   auto* boolean = dynamic_cast<const BoolLiteralExpression*>(&negateExpr->operand());
   ASSERT_NE(boolean, nullptr);
   EXPECT_TRUE(boolean->value());
}

TEST(ParserSimpleTests, ParseCastExpression) {
   std::string code = "x as float\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);
   auto* castExpr = dynamic_cast<const CastExpression*>(&exprStmt->expression());
   ASSERT_NE(castExpr, nullptr);

   auto* id = dynamic_cast<const IdentifierExpression*>(&castExpr->expression());
   ASSERT_NE(id, nullptr);
   EXPECT_EQ(id->name(), "x");

   auto* targetType = dynamic_cast<const FloatTypeNode*>(&castExpr->targetType());
   ASSERT_NE(targetType, nullptr);
}

TEST(ParserSimpleTests, ParseChainedMemberAccessAndCall) {
   std::string code = "user.getName()\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);

   auto* callExpr = dynamic_cast<const CallExpression*>(&exprStmt->expression());
   ASSERT_NE(callExpr, nullptr);
   EXPECT_TRUE(callExpr->arguments().empty());

   auto* memberExpr = dynamic_cast<const MemberAccessExpression*>(&callExpr->callee());
   ASSERT_NE(memberExpr, nullptr);
   EXPECT_EQ(memberExpr->memberName(), "getName");

   auto* baseId = dynamic_cast<const IdentifierExpression*>(&memberExpr->object());
   EXPECT_EQ(baseId->name(), "user");
}

TEST(ParserSimpleTests, ParseIndexAndSliceExpression) {
   std::string code = "matrix[1:5]\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt);
   auto* sliceExpr = dynamic_cast<const SliceExpression*>(&exprStmt->expression());
   ASSERT_NE(sliceExpr, nullptr);

   auto* start = dynamic_cast<const IntLiteralExpression*>(sliceExpr->start());
   ASSERT_NE(start, nullptr);
   EXPECT_EQ(start->value(), 1);

   auto* end = dynamic_cast<const IntLiteralExpression*>(sliceExpr->end());
   ASSERT_NE(end, nullptr);
   EXPECT_EQ(end->value(), 5);
}

TEST(ParserSimpleTests, ParseReturnStatementWithValue) {
   std::string code = "return 0\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* retStmt = dynamic_cast<ReturnStatement*>(stmt);
   ASSERT_NE(retStmt, nullptr);

   ASSERT_NE(retStmt->expression(), nullptr);
   auto* retVal = dynamic_cast<const IntLiteralExpression*>(retStmt->expression());
   EXPECT_EQ(retVal->value(), 0);
}

TEST(ParserSimpleTests, ParseIfStatementWithElseBranch) {
   std::string code =
      "if true {\n"
      "return 1\n"
      "} else {\n"
      "return 0\n"
      "}\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* ifStmt = dynamic_cast<IfStatement*>(stmt);
   ASSERT_NE(ifStmt, nullptr);

   auto* condition = dynamic_cast<const BoolLiteralExpression*>(&ifStmt->condition());
   ASSERT_NE(condition, nullptr);
   EXPECT_TRUE(condition->value());

   ASSERT_EQ(ifStmt->thenBranch().statements().size(), 1);
   EXPECT_NE(dynamic_cast<ReturnStatement*>(ifStmt->thenBranch().statements()[0].get()), nullptr);

   auto* elseBlock = dynamic_cast<const BlockStatement*>(ifStmt->elseBranch());
   ASSERT_NE(elseBlock, nullptr);
   ASSERT_EQ(elseBlock->statements().size(), 1);
   EXPECT_NE(dynamic_cast<ReturnStatement*>(elseBlock->statements()[0].get()), nullptr);
}

TEST(ParserSimpleTests, ParseWhileStatement) {
   std::string code =
      "while keepGoing {\n"
      "continue\n"
      "}\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* whileStmt = dynamic_cast<WhileStatement*>(stmt);
   ASSERT_NE(whileStmt, nullptr);

   auto* condition = dynamic_cast<const IdentifierExpression*>(&whileStmt->condition());
   ASSERT_NE(condition, nullptr);
   EXPECT_EQ(condition->name(), "keepGoing");

   ASSERT_EQ(whileStmt->body().statements().size(), 1);
   EXPECT_NE(dynamic_cast<ContinueStatement*>(whileStmt->body().statements()[0].get()), nullptr);
}

TEST(ParserSimpleTests, ParseForStatement) {
   std::string code =
      "for int item in numbers {\n"
      "break\n"
      "}\n";
   ErrorHandler errHandler("");

   auto program = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(program, nullptr);

   Statement* stmt = firstStatement(program.get());
   ASSERT_NE(stmt, nullptr);

   auto* forStmt = dynamic_cast<ForStatement*>(stmt);
   ASSERT_NE(forStmt, nullptr);

   EXPECT_NE(dynamic_cast<const IntTypeNode*>(&forStmt->variableType()), nullptr);
   EXPECT_EQ(forStmt->variableName(), "item");

   auto* iterable = dynamic_cast<const IdentifierExpression*>(&forStmt->iterable());
   ASSERT_NE(iterable, nullptr);
   EXPECT_EQ(iterable->name(), "numbers");

   ASSERT_EQ(forStmt->body().statements().size(), 1);
   EXPECT_NE(dynamic_cast<BreakStatement*>(forStmt->body().statements()[0].get()), nullptr);
}
