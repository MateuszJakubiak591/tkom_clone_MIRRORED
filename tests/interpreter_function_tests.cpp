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

TEST(InterpreterFunctionTests, ReportsReturnValueMismatchAndConverts) {
   auto result = interpretSource(
      "fun toInt(value: float) -> int {\n"
      "   return value\n"
      "}\n"
      "fun main() -> int {\n"
      "   return toInt(8.0)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 8);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "cannot assign float to int; converting value");
}

TEST(InterpreterFunctionTests, AllowsIntArgumentForFloatParameter) {
   auto result = interpretSource(
      "fun half(value: float) -> int {\n"
      "   return (value / 2.0) as int\n"
      "}\n"
      "fun main() -> int {\n"
      "   return half(8)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 4);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterFunctionTests, ReportsFloatArgumentForIntParameterAndConverts) {
   auto result = interpretSource(
      "fun median3(a: int, b: int, c: int) -> int {\n"
      "   return c\n"
      "}\n"
      "fun main() -> int {\n"
      "   return median3(2, 5, 3.4)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 3);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "cannot pass float to parameter 'c' of type int; converting value");
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
