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

TEST(InterpreterEnvironmentTests, ReportsAssignmentMismatchAndConverts) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   mut int value = 3.4\n"
      "   value = 5.8\n"
      "   return value\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 5);
   ASSERT_EQ(result.errors.size(), 2);
   EXPECT_EQ(result.errors[0].message, "cannot assign float to int; converting value");
   EXPECT_EQ(result.errors[1].message, "cannot assign float to int; converting value");
   EXPECT_NE(
      result.output.find(
         "NON-FATAL Runtime Error at interpreter_test.djm:2:4: "
         "cannot assign float to int; converting value"
      ),
      std::string::npos
   );
}

TEST(InterpreterEnvironmentTests, AllowsNonNegativeIntAssignmentToUint) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   uint value = 5\n"
      "   return value as int\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 5);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterEnvironmentTests, ReportsNegativeIntAssignmentToUint) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   uint value = -5\n"
      "   return value as int\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, -5);
   ASSERT_EQ(result.errors.size(), 3);
   EXPECT_EQ(result.errors[0].message, "cannot assign int to uint; converting value");
   EXPECT_EQ(result.errors[1].message, "int value is outside uint range");
   EXPECT_EQ(result.errors[2].message, "uint value is outside int range");
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
