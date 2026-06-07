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

TEST(InterpreterExpressionTests, ReportsEqualityBetweenDifferentTypesAndReturnsFalse) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   if 2 == \"s\" {\n"
      "      return 1\n"
      "   }\n"
      "   return 0\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 0);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "cannot compare int with string; returning false");
}

TEST(InterpreterExpressionTests, ReportsInequalityBetweenDifferentTypesAndReturnsFalse) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   if 2 != \"s\" {\n"
      "      return 1\n"
      "   }\n"
      "   return 0\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 0);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "cannot compare int with string; returning false");
}

TEST(InterpreterExpressionTests, ReportsRelationalComparisonBetweenDifferentTypesAndReturnsFalse) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   if 2 < \"s\" {\n"
      "      return 1\n"
      "   }\n"
      "   return 0\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 0);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "cannot compare int with string; returning false");
}

TEST(InterpreterExpressionTests, PromotesIntAndFloatArithmeticToFloat) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   return (2 + 3.5) as int\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 5);
   ASSERT_EQ(result.errors.size(), 1);
   EXPECT_EQ(result.errors.back().message, "cannot use int as float in numeric operation; converting value");
}

TEST(InterpreterExpressionTests, PromotesUintAndFloatArithmeticToFloat) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   uint value = 2 as uint\n"
      "   return (value * 3.5) as int\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 7);
   ASSERT_EQ(result.errors.size(), 1);
   EXPECT_EQ(result.errors.back().message, "cannot use uint as float in numeric operation; converting value");
}

TEST(InterpreterExpressionTests, PromotesCharAndIntArithmeticToInt) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   char value = 'A'\n"
      "   return value - 60\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 5);
   ASSERT_EQ(result.errors.size(), 1);
   EXPECT_EQ(result.errors.back().message, "cannot use char as int in numeric operation; converting value");
}

TEST(InterpreterExpressionTests, PromotesCharAndUintArithmeticToUint) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   char value = 'A'\n"
      "   return (value + (2 as uint)) as int\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 67);
   ASSERT_EQ(result.errors.size(), 1);
   EXPECT_EQ(result.errors.back().message, "cannot use char as uint in numeric operation; converting value");
}

TEST(InterpreterExpressionTests, PromotesIntAndUintArithmeticToInt) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   return 10 - (3 as uint)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 7);
   ASSERT_EQ(result.errors.size(), 1);
   EXPECT_EQ(result.errors.back().message, "cannot use uint as int in numeric operation; converting value");
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
