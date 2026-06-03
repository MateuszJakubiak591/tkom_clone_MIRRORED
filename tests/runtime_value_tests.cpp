#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>

#include "diagnostics/ErrorHandler.hpp"
#include "interpreter/RuntimeValue.hpp"
#include "interpreter/Interpreter.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "source/StringSource.hpp"

namespace {
template <typename T>
T getValueData(const Value& value) {
   return std::get<T>(value.data());
}

ProgramPtr parseProgramForRuntimeTest(const std::string& code, ErrorHandler& errorHandler) {
   StringSource source("runtime_value_test.djm", code);
   Lexer lexer(source);
   Parser parser(lexer, &errorHandler);
   return parser.parseProgram();
}
}

TEST(RuntimeTypeTests, ComparesPrimitiveAndListTypes) {
   EXPECT_EQ(RuntimeType::intType(), RuntimeType::intType());
   EXPECT_NE(RuntimeType::intType(), RuntimeType::uintType());

   EXPECT_EQ(
      RuntimeType::listOf(RuntimeType::intType()),
      RuntimeType::listOf(RuntimeType::intType())
   );
   EXPECT_NE(
      RuntimeType::listOf(RuntimeType::intType()),
      RuntimeType::listOf(RuntimeType::stringType())
   );
   EXPECT_EQ(
      RuntimeType::listOf(RuntimeType::listOf(RuntimeType::charType())).toString(),
      "list<list<char>>"
   );
}

TEST(RuntimeValueTests, CreatesDefaultValuesForRuntimeTypes) {
   EXPECT_EQ(getValueData<int64_t>(defaultValueFor(RuntimeType::intType())), 0);
   EXPECT_EQ(getValueData<uint64_t>(defaultValueFor(RuntimeType::uintType())), 0);
   EXPECT_EQ(getValueData<double>(defaultValueFor(RuntimeType::floatType())), 0.0);
   EXPECT_FALSE(getValueData<bool>(defaultValueFor(RuntimeType::boolType())));
   EXPECT_EQ(getValueData<char>(defaultValueFor(RuntimeType::charType())), '\0');
   EXPECT_EQ(getValueData<std::string>(defaultValueFor(RuntimeType::stringType())), "");

   const Value list = defaultValueFor(RuntimeType::listOf(RuntimeType::intType()));
   EXPECT_EQ(list.type(), RuntimeType::listOf(RuntimeType::intType()));
   EXPECT_TRUE(getValueData<ValueList>(list).empty());
}

TEST(RuntimeValueTests, CastsNumericValuesToPrimitiveTypes) {
   EXPECT_EQ(getValueData<uint64_t>(castValue(Value::intValue(42), RuntimeType::uintType())), 42);
   EXPECT_EQ(getValueData<double>(castValue(Value::intValue(-7), RuntimeType::floatType())), -7.0);
   EXPECT_TRUE(getValueData<bool>(castValue(Value::intValue(1), RuntimeType::boolType())));
   EXPECT_FALSE(getValueData<bool>(castValue(Value::intValue(0), RuntimeType::boolType())));
   EXPECT_EQ(getValueData<char>(castValue(Value::intValue(65), RuntimeType::charType())), 'A');

   EXPECT_EQ(getValueData<int64_t>(castValue(Value::uintValue(9), RuntimeType::intType())), 9);
   EXPECT_EQ(getValueData<int64_t>(castValue(Value::floatValue(12.75), RuntimeType::intType())), 12);
   EXPECT_EQ(getValueData<uint64_t>(castValue(Value::floatValue(12.75), RuntimeType::uintType())), 12);
}

TEST(RuntimeValueTests, CastsBoolAndCharValuesToPrimitiveTypes) {
   EXPECT_EQ(getValueData<int64_t>(castValue(Value::boolValue(true), RuntimeType::intType())), 1);
   EXPECT_EQ(getValueData<uint64_t>(castValue(Value::boolValue(false), RuntimeType::uintType())), 0);
   EXPECT_EQ(getValueData<double>(castValue(Value::boolValue(true), RuntimeType::floatType())), 1.0);
   EXPECT_EQ(getValueData<char>(castValue(Value::boolValue(true), RuntimeType::charType())), 1);

   EXPECT_EQ(getValueData<int64_t>(castValue(Value::charValue('A'), RuntimeType::intType())), 65);
   EXPECT_EQ(getValueData<uint64_t>(castValue(Value::charValue('A'), RuntimeType::uintType())), 65);
   EXPECT_EQ(getValueData<double>(castValue(Value::charValue('A'), RuntimeType::floatType())), 65.0);
   EXPECT_TRUE(getValueData<bool>(castValue(Value::charValue('A'), RuntimeType::boolType())));
}

TEST(RuntimeValueTests, CastsPrimitiveValuesToString) {
   EXPECT_EQ(getValueData<std::string>(castValue(Value::intValue(-42), RuntimeType::stringType())), "-42");
   EXPECT_EQ(getValueData<std::string>(castValue(Value::uintValue(42), RuntimeType::stringType())), "42");
   EXPECT_EQ(getValueData<std::string>(castValue(Value::boolValue(true), RuntimeType::stringType())), "true");
   EXPECT_EQ(getValueData<std::string>(castValue(Value::charValue('x'), RuntimeType::stringType())), "x");
}

TEST(RuntimeValueTests, CastsStringValuesToNumericTypes) {
   EXPECT_EQ(getValueData<int64_t>(castValue(Value::stringValue("-1234"), RuntimeType::intType())), -1234);
   EXPECT_EQ(getValueData<uint64_t>(castValue(Value::stringValue("4294967295"), RuntimeType::uintType())), 4294967295ULL);
   EXPECT_DOUBLE_EQ(getValueData<double>(castValue(Value::stringValue("-123.456"), RuntimeType::floatType())), -123.456);
   EXPECT_DOUBLE_EQ(getValueData<double>(castValue(Value::stringValue("1000.25"), RuntimeType::floatType())), 1000.25);

   EXPECT_EQ(stringToInt("-1234"), -1234);
   EXPECT_EQ(stringToUint("4294967295"), 4294967295ULL);
   EXPECT_DOUBLE_EQ(stringToFloat("-123.456"), -123.456);
   EXPECT_EQ(stringToInt("-9223372036854775808"), std::numeric_limits<int64_t>::min());
}

TEST(RuntimeValueTests, RejectsInvalidStringNumericCasts) {
   EXPECT_THROW(castValue(Value::stringValue("12abc"), RuntimeType::intType()), RuntimeValueInvalidStringCast);
   EXPECT_THROW(castValue(Value::stringValue("1_2"), RuntimeType::intType()), RuntimeValueInvalidStringCast);
   EXPECT_THROW(castValue(Value::stringValue("1_000.25"), RuntimeType::floatType()), RuntimeValueInvalidStringCast);
   EXPECT_THROW(castValue(Value::stringValue(""), RuntimeType::intType()), RuntimeValueInvalidStringCast);
   EXPECT_THROW(castValue(Value::stringValue("-1"), RuntimeType::uintType()), RuntimeValueInvalidStringCast);
   EXPECT_THROW(castValue(Value::stringValue("12.3.4"), RuntimeType::floatType()), RuntimeValueInvalidStringCast);
}

TEST(RuntimeValueTests, ThrowsOutOfRangeWithWrappedValue) {
   try {
      castValue(Value::intValue(1000), RuntimeType::charType());
      FAIL() << "expected RuntimeValueOutOfRange";
   } catch (const RuntimeValueOutOfRange& error) {
      EXPECT_EQ(error.wrappedValue().type(), RuntimeType::charType());
      EXPECT_EQ(static_cast<unsigned char>(getValueData<char>(error.wrappedValue())), 232);
   }

   try {
      castValue(Value::stringValue("9223372036854775808"), RuntimeType::intType());
      FAIL() << "expected RuntimeValueOutOfRange";
   } catch (const RuntimeValueOutOfRange& error) {
      EXPECT_EQ(error.wrappedValue().type(), RuntimeType::intType());
      EXPECT_EQ(getValueData<int64_t>(error.wrappedValue()), std::numeric_limits<int64_t>::min());
   }

   try {
      castValue(Value::stringValue("18446744073709551616"), RuntimeType::uintType());
      FAIL() << "expected RuntimeValueOutOfRange";
   } catch (const RuntimeValueOutOfRange& error) {
      EXPECT_EQ(error.wrappedValue().type(), RuntimeType::uintType());
      EXPECT_EQ(getValueData<uint64_t>(error.wrappedValue()), 0);
   }
}

TEST(RuntimeValueTests, CastsListsElementByElement) {
   ValueList values;
   values.push_back(Value::stringValue("1"));
   values.push_back(Value::stringValue("2"));
   values.push_back(Value::stringValue("3"));

   const Value result = castValue(
      Value::listValue(RuntimeType::stringType(), std::move(values)),
      RuntimeType::listOf(RuntimeType::intType())
   );

   ASSERT_EQ(result.type(), RuntimeType::listOf(RuntimeType::intType()));
   const auto& elements = getValueData<ValueList>(result);
   ASSERT_EQ(elements.size(), 3);
   EXPECT_EQ(getValueData<int64_t>(elements[0]), 1);
   EXPECT_EQ(getValueData<int64_t>(elements[1]), 2);
   EXPECT_EQ(getValueData<int64_t>(elements[2]), 3);
}

TEST(RuntimeValueTests, ChecksEqualityForPrimitiveAndListValues) {
   EXPECT_TRUE(valuesEqual(Value::intValue(7), Value::intValue(7)));
   EXPECT_FALSE(valuesEqual(Value::intValue(7), Value::intValue(8)));
   EXPECT_FALSE(valuesEqual(Value::intValue(7), Value::uintValue(7)));

   ValueList left;
   left.push_back(Value::intValue(1));
   left.push_back(Value::intValue(2));

   ValueList right;
   right.push_back(Value::intValue(1));
   right.push_back(Value::intValue(2));

   EXPECT_TRUE(valuesEqual(
      Value::listValue(RuntimeType::intType(), std::move(left)),
      Value::listValue(RuntimeType::intType(), std::move(right))
   ));
}

TEST(RuntimeValueTests, ComparesOrderedValues) {
   EXPECT_LT(compareValues(Value::intValue(1), Value::intValue(2)), 0);
   EXPECT_EQ(compareValues(Value::intValue(2), Value::intValue(2)), 0);
   EXPECT_GT(compareValues(Value::intValue(3), Value::intValue(2)), 0);

   EXPECT_LT(compareValues(Value::stringValue("abc"), Value::stringValue("abd")), 0);
   EXPECT_THROW(compareValues(Value::intValue(1), Value::uintValue(1)), std::runtime_error);
}

TEST(RuntimeValueInterpreterTests, InvalidStringCastReportsRuntimeErrorAndStops) {
   const std::string code =
      "fun main() -> int {\n"
      "   string s = \"123abc\"\n"
      "   int i = s as int\n"
      "   return 37\n"
      "}\n";
   ErrorHandler errorHandler(code);
   auto program = parseProgramForRuntimeTest(code, errorHandler);
   ASSERT_NE(program, nullptr);
   ASSERT_FALSE(errorHandler.hasErrors());

   std::ostringstream output;
   Interpreter interpreter(&errorHandler, &output);

   EXPECT_EQ(interpreter.interpret(*program), 1);
   ASSERT_TRUE(errorHandler.hasErrors());
   EXPECT_NE(errorHandler.errors().back().message.find("cannot cast string '123abc' to int"), std::string::npos);
}

TEST(RuntimeValueInterpreterTests, OutOfRangeCastReportsRuntimeErrorAndContinues) {
   const std::string code =
      "fun main() -> int {\n"
      "   int i = 1000 as char as int\n"
      "   return 37\n"
      "}\n";
   ErrorHandler errorHandler(code);
   auto program = parseProgramForRuntimeTest(code, errorHandler);
   ASSERT_NE(program, nullptr);
   ASSERT_FALSE(errorHandler.hasErrors());

   std::ostringstream output;
   Interpreter interpreter(&errorHandler, &output);

   EXPECT_EQ(interpreter.interpret(*program), 37);
   ASSERT_TRUE(errorHandler.hasErrors());
   EXPECT_NE(errorHandler.errors().back().message.find("outside char range"), std::string::npos);
}

TEST(RuntimeValueInterpreterTests, NullErrorHandlerIsAcceptedByInterpreter) {
   const std::string code =
      "fun main() -> int {\n"
      "   string s = \"123abc\"\n"
      "   int i = s as int\n"
      "   return 37\n"
      "}\n";
   ErrorHandler parseErrorHandler(code);
   auto program = parseProgramForRuntimeTest(code, parseErrorHandler);
   ASSERT_NE(program, nullptr);
   ASSERT_FALSE(parseErrorHandler.hasErrors());

   std::ostringstream output;
   Interpreter interpreter(nullptr, &output);

   EXPECT_EQ(interpreter.interpret(*program), 1);
   EXPECT_TRUE(output.str().empty());
}
