#include <gtest/gtest.h>

#include "InterpreterTestUtils.hpp"

TEST(InterpreterControlFlowTests, ChoosesIfElseBranch) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   if false {\n"
      "      return 1\n"
      "   } else {\n"
      "      return 7\n"
      "   }\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 7);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterControlFlowTests, ExecutesWhileLoop) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   mut int i = 0\n"
      "   mut int sum = 0\n"
      "   while i < 5 {\n"
      "      sum = sum + i\n"
      "      i = i + 1\n"
      "   }\n"
      "   return sum\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 10);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterControlFlowTests, HandlesBreakAndContinueInLoops) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   mut int i = 0\n"
      "   mut int sum = 0\n"
      "   while i < 10 {\n"
      "      i = i + 1\n"
      "      if i == 2 {\n"
      "         continue\n"
      "      }\n"
      "      if i == 5 {\n"
      "         break\n"
      "      }\n"
      "      sum = sum + i\n"
      "   }\n"
      "   return sum\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 8);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterControlFlowTests, IteratesForLoopOverList) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   mut int sum = 0\n"
      "   for int value in [2, 3, 4] {\n"
      "      sum = sum + value\n"
      "   }\n"
      "   return sum\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 9);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterControlFlowTests, ContinuesForLoopWithEmptyListWhenIterableIsNotList) {
   auto result = interpretSource(
      "fun notList() -> int {\n"
      "   return 12\n"
      "}\n"
      "fun main() -> int {\n"
      "   mut int total = 0\n"
      "   for int value in notList() {\n"
      "      total = total + 1\n"
      "   }\n"
      "   return total\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 0);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(
      result.errors.back().message,
      "for iterable must be list<int>; continuing with an empty list"
   );
}

TEST(InterpreterControlFlowTests, CorrectsForLoopVariableTypeToIterableElementType) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   mut int sum = 0\n"
      "   for string value in [2, 3] {\n"
      "      sum = sum + value\n"
      "   }\n"
      "   return sum\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 5);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(
      result.errors.back().message,
      "for loop variable type string does not match iterable element type int; continuing with corrected loop type"
   );
}
