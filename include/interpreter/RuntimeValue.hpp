#pragma once

/// @file RuntimeValue.hpp
/// Type-safe runtime values, storage cells and conversion helpers.

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "syntax/Type.hpp"

/// Runtime representation of a DJM type, including recursively nested list types.
class RuntimeType {
public:
   /// Closed set of type categories supported by the interpreter.
   enum class Kind {
      Void,
      Int,
      Uint,
      Float,
      Bool,
      Char,
      String,
      List
   };

   static RuntimeType voidType();
   static RuntimeType intType();
   static RuntimeType uintType();
   static RuntimeType floatType();
   static RuntimeType boolType();
   static RuntimeType charType();
   static RuntimeType stringType();
   /// Creates a list type whose element type remains valid with the returned object.
   static RuntimeType listOf(RuntimeType elementType);

   /// Returns the top-level category of this type.
   Kind kind() const;
   /// Returns the list element type; throws when called for a non-list type.
   const RuntimeType& elementType() const;

   bool operator==(const RuntimeType& other) const;
   bool operator!=(const RuntimeType& other) const;
   std::string toString() const;

private:
   RuntimeType(Kind kind, std::shared_ptr<RuntimeType> elementType = nullptr);

   Kind kind_;
   std::shared_ptr<RuntimeType> elementType_;
};

RuntimeType runtimeTypeFromNode(const TypeNode& node);

class Value;
using ValueList = std::vector<Value>;

/// Immutable, type-consistent runtime value created only through named factories.
class Value {
public:
   using Data = std::variant<
      std::monostate,
      int64_t,
      uint64_t,
      double,
      bool,
      char,
      std::string,
      ValueList
   >;

   static Value voidValue();
   static Value intValue(int64_t value);
   static Value uintValue(uint64_t value);
   static Value floatValue(double value);
   static Value boolValue(bool value);
   static Value charValue(char value);
   static Value stringValue(std::string value);
   /// Creates a homogeneous list value with the explicitly provided element type.
   static Value listValue(RuntimeType elementType, ValueList elements);

   /// Returns the DJM runtime type matching the active variant alternative.
   const RuntimeType& type() const;
   /// Exposes immutable payload data for type-directed interpreter operations.
   const Data& data() const;

   std::string toString() const;

private:
   Value(RuntimeType type, Data data);

   RuntimeType type_;
   Data data_;
};

/// Variable storage cell that adds mutability semantics to an immutable Value.
class ValueObject {
public:
   ValueObject(Value value, bool isMutable);

   const RuntimeType& type() const;
   const Value& value() const;
   bool isMutable() const;

   /// Replaces the stored value; the caller must validate type and mutability first.
   void assign(Value value);

private:
   Value value_;
   bool isMutable_;
};

using ValueRef = std::shared_ptr<ValueObject>;

class RuntimeValueInvalidStringCast final : public std::runtime_error {
public:
   explicit RuntimeValueInvalidStringCast(const std::string& message);
};

/// Signals a lossy numeric cast and carries the deterministic wrapped result.
class RuntimeValueOutOfRange final : public std::runtime_error {
public:
   RuntimeValueOutOfRange(const std::string& message, Value wrappedValue);

   const Value& wrappedValue() const;

private:
   Value wrappedValue_;
};

int64_t stringToInt(const std::string& text);
uint64_t stringToUint(const std::string& text);
double stringToFloat(const std::string& text);

Value defaultValueFor(const RuntimeType& type);
Value cloneValue(const Value& value);
bool valuesEqual(const Value& left, const Value& right);
int compareValues(const Value& left, const Value& right);
/// Explicitly converts a value and reports invalid or out-of-range conversions by exception.
Value castValue(const Value& value, const RuntimeType& targetType);
