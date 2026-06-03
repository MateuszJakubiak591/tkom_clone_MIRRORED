#include <gtest/gtest.h>

#include "InterpreterTestUtils.hpp"

TEST(InterpreterListOperatorTests, MapsListUsingThisExpression) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> doubled = [1, 2, 3] |> (this * 2)\n"
      "   return doubled[2]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 6);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, FiltersListUsingThisExpression) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> selected = [1, 2, 3, 4] ? (this > 2)\n"
      "   return count selected\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 2);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, GroupsListsByCommonElements) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> common = [1, 2, 3, 4] % [2, 4, 6]\n"
      "   return common[1]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 4);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, SlicesLists) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = [10, 20, 30, 40][1:3]\n"
      "   return values[1]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 30);
   EXPECT_TRUE(result.errors.empty());
}
