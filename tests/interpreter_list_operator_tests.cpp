#include <gtest/gtest.h>

#include "InterpreterTestUtils.hpp"

TEST(InterpreterListOperatorTests, ConcatenatesListsWithPlus) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = [1, 2] + [3, 4]\n"
      "   return values[2]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 3);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, SubtractsFirstMatchingElementsWithMinus) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = [1, 2, 3, 2] - [2, 5]\n"
      "   return values[2]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 2);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, SubtractsListsLeftAssociatively) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = [1, 2, 3, 2] - [2, 5] - [3]\n"
      "   return values[1]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 2);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, IntersectsListsWithMultiplicityUsingMultiply) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = [1, 2, 2, 3, 2] * [2, 2, 4]\n"
      "   return count values\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 2);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, PreservesLeftOrderForListIntersection) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = [3, 1, 2, 1] * [1, 3]\n"
      "   return values[0] * 10 + values[1]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 31);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, GroupsListByThisValue) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<list<int>> grouped = [1, 2, 1, 3, 2] % this\n"
      "   return grouped[0][1] * 10 + grouped[1][1]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 12);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, GroupsListByComputedExpression) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<list<int>> grouped = [1, 2, 3, 4, 5] % (this > 2)\n"
      "   return count grouped[1]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 3);
   EXPECT_TRUE(result.errors.empty());
}

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

TEST(InterpreterListOperatorTests, ChecksWhetherListContainsAnotherList) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   if [1, 2, 3] contains [2, 3] {\n"
      "      return 9\n"
      "   }\n"
      "   return 0\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 9);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, RejectsContainsWhenRightListHasMissingElement) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   if [1, 2, 3] contains [2, 4] {\n"
      "      return 1\n"
      "   }\n"
      "   return 8\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 8);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, CountsLists) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   return count [10, 20, 30]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 3);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, SkipsMixedTypeListLiteralElements) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = [1, \"bad\", 2]\n"
      "   return values[1]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 2);
   ASSERT_FALSE(result.errors.empty());
   EXPECT_EQ(
      result.errors.back().message,
      "mixed type list literal is not supported; element skipped"
   );
}

TEST(InterpreterListOperatorTests, ReversesLists) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = reverse [1, 2, 3]\n"
      "   return values[0]\n"
      "}\n"
   );

   EXPECT_EQ(result.exitCode, 3);
   EXPECT_TRUE(result.errors.empty());
}

TEST(InterpreterListOperatorTests, FlattensOneNestedListLevel) {
   auto result = interpretSource(
      "fun main() -> int {\n"
      "   list<int> values = flatten [[1, 2], [3], [4, 5]]\n"
      "   return values[3]\n"
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
