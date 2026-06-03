#include <gtest/gtest.h>

#include "InterpreterTestUtils.hpp"

TEST(InterpreterFunctionTests, CallsUserFunctionWithArguments) {
   auto result = interpretSource(
      "fun add(a: int, b: int) -> int {\n"
      "   return a + b\n"
      "}\n"
      "fun main() -> int {\n"
      "   return add(20, 22)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 42);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterFunctionTests, CoercesReturnValue) {
   auto result = interpretSource(
      "fun toInt(value: float) -> int {\n"
      "   return value\n"
      "}\n"
      "fun main() -> int {\n"
      "   return toInt(8.0)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 8);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterFunctionTests, RejectsFloatArgumentForIntParameter) {
   auto result = interpretSource(
      "fun median3(a: int, b: int, c: int) -> int {\n"
      "   return c\n"
      "}\n"
      "fun main() -> int {\n"
      "   return median3(2, 5, 3.4)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 1);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "cannot pass float to parameter 'c' of type int");
}

TEST(InterpreterFunctionTests, KeepsFunctionLocalsIsolatedBetweenCalls) {
   auto result = interpretSource(
      "fun next(value: int) -> int {\n"
      "   mut int local = value\n"
      "   local = local + 1\n"
      "   return local\n"
      "}\n"
      "fun main() -> int {\n"
      "   return next(1) + next(10)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 13);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterFunctionTests, ReportsWrongArgumentCount) {
   auto result = interpretSource(
      "fun add(a: int, b: int) -> int {\n"
      "   return a + b\n"
      "}\n"
      "fun main() -> int {\n"
      "   return add(1)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 1);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "wrong number of arguments for function: add");
}
