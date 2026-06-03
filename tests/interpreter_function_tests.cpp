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

TEST(InterpreterFunctionTests, CoercesArgumentsAndReturnValue) {
   auto result = interpretSource(
      "fun toInt(value: float) -> int {\n"
      "   return value\n"
      "}\n"
      "fun main() -> int {\n"
      "   return toInt(8)\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 8);
   EXPECT_TRUE(result.errors.empty());
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
