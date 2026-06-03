#include <gtest/gtest.h>

#include "InterpreterTestUtils.hpp"

TEST(InterpreterEnvironmentTests, ReadsGlobalConstantsFromMainProgram) {
   auto result = interpretSource(
      "int BASE = 12\n"
      "fun main() -> int {\n"
      "   return BASE + 5\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 17);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterEnvironmentTests, AssignsMutableLocalVariables) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   mut int value = 10\n"
      "   value = value + 4\n"
      "   return value\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 14);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterEnvironmentTests, RejectsAssignmentToImmutableVariable) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   int value = 10\n"
      "   value = 11\n"
      "   return value\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 10);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(result.errors.back().message, "cannot assign to immutable variable; assignment skipped");
}

TEST(InterpreterEnvironmentTests, KeepsBlockScopeLocal) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   mut int value = 1\n"
      "   {\n"
      "      int value = 9\n"
      "   }\n"
      "   return value\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 1);
   EXPECT_TRUE(result.errors.empty());
}
