#include <gtest/gtest.h>
#include "TestsUtils.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Expressions.hpp"

TEST(ParserSimpleTests, ParseImmutableVariableWithoutInitializer) {
   std::string code = "string name\n";
   ErrorHandler errHandler("");

   StmtPtr stmt = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(stmt, nullptr);

   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt.get());
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

   StmtPtr stmt = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(stmt, nullptr);

   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt.get());
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

   StmtPtr stmt = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(stmt, nullptr);

   auto* assignStmt = dynamic_cast<AssignmentStatement*>(stmt.get());
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

   StmtPtr stmt = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(stmt, nullptr);

   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt.get());
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

   StmtPtr stmt = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors());
   ASSERT_NE(stmt, nullptr);

   auto* exprStmt = dynamic_cast<ExpressionStatement*>(stmt.get());
   auto* mapExpr = dynamic_cast<const MapExpression*>(&exprStmt->expression());
   ASSERT_NE(mapExpr, nullptr);
   
   auto* left = dynamic_cast<const IdentifierExpression*>(&mapExpr->left());
   EXPECT_EQ(left->name(), "data");

   auto* right = dynamic_cast<const IdentifierExpression*>(&mapExpr->right());
   EXPECT_EQ(right->name(), "transform");
}

TEST(ParserSimpleTests, ParseUserDefinedTypeWithConstructorCall) {
   std::string code = "mut User user = User()\n";
   ErrorHandler errHandler("");

   StmtPtr stmt = parseString(code, &errHandler);

   EXPECT_FALSE(errHandler.hasErrors()) << "Parser zaszalał i zgłosił błędy!";
   ASSERT_NE(stmt, nullptr);

   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt.get());
   ASSERT_NE(varDecl, nullptr) << "Węzeł główny to nie VariableDeclarationStatement!";

   EXPECT_TRUE(varDecl->isMutable());

   auto* userType = dynamic_cast<const UserTypeNode*>(&varDecl->type());
   ASSERT_NE(userType, nullptr) << "Pole type_ to nie UserTypeNode!";
   EXPECT_EQ(userType->name(), "User");

   ASSERT_EQ(varDecl->names().size(), 1);
   EXPECT_EQ(varDecl->names()[0].name, "user");

   ASSERT_NE(varDecl->initializer(), nullptr);
   auto* callExpr = dynamic_cast<const CallExpression*>(varDecl->initializer());
   ASSERT_NE(callExpr, nullptr) << "Inicjalizator to nie CallExpression!";

   auto* calleeId = dynamic_cast<const IdentifierExpression*>(&callExpr->callee());
   ASSERT_NE(calleeId, nullptr) << "Obiekt wywoływany (callee_) to nie IdentifierExpression!";
   EXPECT_EQ(calleeId->name(), "User");

   EXPECT_TRUE(callExpr->arguments().empty());
}