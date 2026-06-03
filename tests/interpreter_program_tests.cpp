#include <gtest/gtest.h>

#include "InterpreterTestUtils.hpp"

TEST(InterpreterProgramTests, ReturnsMainResultAsProcessExitCode) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   return 42\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 42);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterProgramTests, UsesDefaultReturnValueWhenFunctionDoesNotReturnExplicitly) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 0);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterProgramTests, PassesProgramArgumentsAsStringList) {
   auto result = interpretSource(
      "fun main(args: list<string>) -> int {\n"
      "   return count args\n"
      "}\n",
      {"first", "second", "third"}
   );

   EXPECT_EQ(result.exitCode, 3);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterProgramTests, RejectsMainWithUnsupportedArity) {
   auto result = interpretSource(
      "fun main(a: int, b: int) -> int {\n"
      "   return a + b\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 1);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "main must take zero arguments or list<string>");
}

TEST(InterpreterProgramTests, RejectsMainWithUnsupportedSingleParameterType) {
   auto result = interpretSource(
      "fun main(x: int) -> int {\n"
      "   return x\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 1);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "main must take zero arguments or list<string>");
}
