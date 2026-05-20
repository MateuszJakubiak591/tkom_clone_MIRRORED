#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "source/StringSource.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "diagnostics/ErrorHandler.hpp"

#include "syntax/Expressions.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Declarations.hpp"
#include "syntax/Type.hpp"

namespace {

ExprPtr parseExpressionFromString(const std::string& code, ErrorHandler& errorHandler) {
   StringSource source("test.djm", code);
   Lexer lexer(source);
   Parser parser(lexer, &errorHandler);

   return parser.parseExpression();
}

const IdentifierExpression* asIdentifier(const Expression& expression) {
   return dynamic_cast<const IdentifierExpression*>(&expression);
}

const IntLiteralExpression* asIntLiteral(const Expression& expression) {
   return dynamic_cast<const IntLiteralExpression*>(&expression);
}

const ThisExpression* asThis(const Expression& expression) {
   return dynamic_cast<const ThisExpression*>(&expression);
}

const MemberAccessExpression* asMemberAccess(const Expression& expression) {
   return dynamic_cast<const MemberAccessExpression*>(&expression);
}

}

TEST(ParserComplexExpressionTests, ParsesCountFilterMapCallExpression) {
   ErrorHandler errorHandler;

   auto expression = parseExpressionFromString(
      "count ((users ? (this.age > 18 && this.isActive)) |> toThePowerOf(this.age, 2))",
      errorHandler
   );

   ASSERT_FALSE(errorHandler.hasErrors());
   ASSERT_NE(expression, nullptr);

   const auto* count = dynamic_cast<const CountExpression*>(expression.get());
   ASSERT_NE(count, nullptr);

   const auto* map = dynamic_cast<const MapExpression*>(&count->operand());
   ASSERT_NE(map, nullptr);

   // users ? (this.age > 18 && this.isActive)
   const auto* filter = dynamic_cast<const FilterExpression*>(&map->left());
   ASSERT_NE(filter, nullptr);

   const auto* filteredList = asIdentifier(filter->left());
   ASSERT_NE(filteredList, nullptr);
   EXPECT_EQ(filteredList->name(), "users");

   const auto* condition = dynamic_cast<const LogicalAndExpression*>(&filter->right());
   ASSERT_NE(condition, nullptr);

   // this.age > 18
   const auto* greater = dynamic_cast<const GreaterExpression*>(&condition->left());
   ASSERT_NE(greater, nullptr);

   const auto* ageAccess = asMemberAccess(greater->left());
   ASSERT_NE(ageAccess, nullptr);
   EXPECT_EQ(ageAccess->memberName(), "age");

   const auto* ageObject = asThis(ageAccess->object());
   ASSERT_NE(ageObject, nullptr);

   const auto* ageLimit = asIntLiteral(greater->right());
   ASSERT_NE(ageLimit, nullptr);
   EXPECT_EQ(ageLimit->value(), 18);

   // this.isActive
   const auto* activeAccess = asMemberAccess(condition->right());
   ASSERT_NE(activeAccess, nullptr);
   EXPECT_EQ(activeAccess->memberName(), "isActive");

   const auto* activeObject = asThis(activeAccess->object());
   ASSERT_NE(activeObject, nullptr);


   // toThePowerOf(this.age, 2)
   const auto* call = dynamic_cast<const CallExpression*>(&map->right());
   ASSERT_NE(call, nullptr);

   const auto* callee = asIdentifier(call->callee());
   ASSERT_NE(callee, nullptr);
   EXPECT_EQ(callee->name(), "toThePowerOf");

   ASSERT_EQ(call->arguments().size(), 2);

   const auto* firstArg = asMemberAccess(*call->arguments()[0]);
   ASSERT_NE(firstArg, nullptr);
   EXPECT_EQ(firstArg->memberName(), "age");

   const auto* firstArgObject = asThis(firstArg->object());
   ASSERT_NE(firstArgObject, nullptr);

   const auto* secondArg = dynamic_cast<const IntLiteralExpression*>(call->arguments()[1].get());
   ASSERT_NE(secondArg, nullptr);
   EXPECT_EQ(secondArg->value(), 2);
}

TEST(ParserComplexExpressionTests, ParsesCountOfListIntersectionComparison) {
   ErrorHandler errorHandler;

   auto expression = parseExpressionFromString(
      "count (list1 * list2) > 0",
      errorHandler
   );

   ASSERT_FALSE(errorHandler.hasErrors());
   ASSERT_NE(expression, nullptr);

   const auto* greater = dynamic_cast<const GreaterExpression*>(expression.get());
   ASSERT_NE(greater, nullptr);

   const auto* count = dynamic_cast<const CountExpression*>(&greater->left());
   ASSERT_NE(count, nullptr);

   const auto* intersection = dynamic_cast<const MultiplyExpression*>(&count->operand());
   ASSERT_NE(intersection, nullptr);

   const auto* leftList = asIdentifier(intersection->left());
   ASSERT_NE(leftList, nullptr);
   EXPECT_EQ(leftList->name(), "list1");

   const auto* rightList = asIdentifier(intersection->right());
   ASSERT_NE(rightList, nullptr);
   EXPECT_EQ(rightList->name(), "list2");

   const auto* zero = asIntLiteral(greater->right());
   ASSERT_NE(zero, nullptr);
   EXPECT_EQ(zero->value(), 0);
}

TEST(ParserComplexExpressionTests, ParsesReverseFlattenExpression) {
   ErrorHandler errorHandler;

   auto expression = parseExpressionFromString(
      "reverse flatten nested",
      errorHandler
   );

   ASSERT_FALSE(errorHandler.hasErrors());
   ASSERT_NE(expression, nullptr);

   const auto* reverse = dynamic_cast<const ReverseExpression*>(expression.get());
   ASSERT_NE(reverse, nullptr);

   const auto* flatten = dynamic_cast<const FlattenExpression*>(&reverse->operand());
   ASSERT_NE(flatten, nullptr);

   const auto* nested = asIdentifier(flatten->operand());
   ASSERT_NE(nested, nullptr);
   EXPECT_EQ(nested->name(), "nested");
}

