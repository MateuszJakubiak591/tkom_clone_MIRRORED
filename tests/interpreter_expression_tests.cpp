#include <gtest/gtest.h>

#include "InterpreterTestUtils.hpp"

TEST(InterpreterExpressionTests, EvaluatesArithmeticPrecedence) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   return 2 + 3 * 4 - 5\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 9);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterExpressionTests, EvaluatesComparisonAndLogicalOperators) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   if 2 < 3 && 4 >= 4 {\n"
      "      return 11\n"
      "   }\n"
      "   return 0\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 11);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterExpressionTests, ConcatenatesStringsAndCountsResult) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   string text = \"ab\" + \"cd\"\n"
      "   return count text\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 4);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterExpressionTests, CastsStringToIntInExpression) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   string value = \"37\"\n"
      "   return value as int\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 37);
   EXPECT_TRUE(result.errors.empty());
}
