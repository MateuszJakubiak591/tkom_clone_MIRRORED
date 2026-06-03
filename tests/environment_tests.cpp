#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#include "interpreter/Environment.hpp"

namespace {
SourceLocation loc(int line = 1, int column = 1) {
   return SourceLocation{"environment_test.djm", line, column};
}

int64_t intData(const ValueRef& value) {
   return std::get<int64_t>(value->value().data());
}
}

TEST(ScopeTests, DefinesAndFindsLocalValues) {
   Scope scope;
   auto value = std::make_shared<ValueObject>(Value::intValue(7), true);

   EXPECT_TRUE(scope.define("x", value));
   EXPECT_FALSE(scope.define("x", std::make_shared<ValueObject>(Value::intValue(8), true)));
   EXPECT_EQ(scope.findLocal("x"), value);
   EXPECT_EQ(scope.findLocal("missing"), nullptr);
}

TEST(EnvironmentTests, StoresAndFindsGlobalVariablesWithoutCallContext) {
   Environment environment;
   environment.defineVariable("answer", std::make_shared<ValueObject>(Value::intValue(42), false), loc());

   ValueRef found = environment.findVariable("answer", loc());

   ASSERT_NE(found, nullptr);
   EXPECT_EQ(intData(found), 42);
   EXPECT_FALSE(found->isMutable());
}

TEST(EnvironmentTests, UsesInnermostScopeBeforeOuterScopesAndGlobals) {
   Environment environment;
   environment.defineVariable("value", std::make_shared<ValueObject>(Value::intValue(1), true), loc());

   environment.pushCallContext();
   environment.defineVariable("value", std::make_shared<ValueObject>(Value::intValue(2), true), loc());
   EXPECT_EQ(intData(environment.findVariable("value", loc())), 2);

   environment.pushScope();
   environment.defineVariable("value", std::make_shared<ValueObject>(Value::intValue(3), true), loc());
   EXPECT_EQ(intData(environment.findVariable("value", loc())), 3);

   environment.popScope();
   EXPECT_EQ(intData(environment.findVariable("value", loc())), 2);

   environment.popCallContext();
   EXPECT_EQ(intData(environment.findVariable("value", loc())), 1);
}

TEST(EnvironmentTests, RejectsDuplicateVariableInCurrentScope) {
   Environment environment;
   environment.defineVariable("x", std::make_shared<ValueObject>(Value::intValue(1), true), loc(2, 3));

   EXPECT_THROW(
      environment.defineVariable("x", std::make_shared<ValueObject>(Value::intValue(2), true), loc(2, 7)),
      RuntimeError
   );
}

TEST(EnvironmentTests, ReturnsNullFromTryFindAndThrowsFromFindForMissingVariable) {
   Environment environment;

   EXPECT_EQ(environment.tryFindVariable("missing"), nullptr);
   EXPECT_THROW(environment.findVariable("missing", loc(4, 5)), RuntimeError);
}

TEST(EnvironmentTests, RequiresCallContextBeforePushingLocalScope) {
   Environment environment;

   EXPECT_FALSE(environment.hasCallContext());
   EXPECT_THROW(environment.pushScope(), RuntimeError);
}

TEST(EnvironmentTests, AddsBuiltinFunctions) {
   Environment environment;
   environment.addBuiltins();

   Callable* print = environment.tryFindFunction("print");
   Callable* println = environment.tryFindFunction("println");

   ASSERT_NE(print, nullptr);
   ASSERT_NE(println, nullptr);
   EXPECT_EQ(print->arity(), 1);
   EXPECT_EQ(println->arity(), 1);
   EXPECT_EQ(print->returnType(), RuntimeType::voidType());
   EXPECT_EQ(println->returnType(), RuntimeType::voidType());
   EXPECT_EQ(environment.tryFindFunction("missing"), nullptr);
}
