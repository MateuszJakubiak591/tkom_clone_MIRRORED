#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "syntax/Type.hpp"

struct RuntimeType {
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

   Kind kind = Kind::Void;
   std::shared_ptr<RuntimeType> elementType;

   static RuntimeType voidType();
   static RuntimeType intType();
   static RuntimeType uintType();
   static RuntimeType floatType();
   static RuntimeType boolType();
   static RuntimeType charType();
   static RuntimeType stringType();
   static RuntimeType listOf(RuntimeType elementType);

   bool operator==(const RuntimeType& other) const;
   bool operator!=(const RuntimeType& other) const;
   std::string toString() const;
};

RuntimeType runtimeTypeFromNode(const TypeNode& node);

struct Value;
using ValueList = std::vector<Value>;

struct Value {
   RuntimeType type;
   std::variant<
      std::monostate,
      int64_t,
      uint64_t,
      double,
      bool,
      char,
      std::string,
      ValueList
   > data;

   static Value voidValue();
   static Value intValue(int64_t value);
   static Value uintValue(uint64_t value);
   static Value floatValue(double value);
   static Value boolValue(bool value);
   static Value charValue(char value);
   static Value stringValue(std::string value);
   static Value listValue(RuntimeType elementType, ValueList elements);

   std::string toString() const;
};

class ValueObject {
public:
   ValueObject(Value value, bool isMutable);

   const RuntimeType& type() const;
   const Value& value() const;
   bool isMutable() const;

   void assign(Value value);

private:
   Value value_;
   bool isMutable_;
};

using ValueRef = std::shared_ptr<ValueObject>;

Value defaultValueFor(const RuntimeType& type);
Value cloneValue(const Value& value);
bool valuesEqual(const Value& left, const Value& right);
int compareValues(const Value& left, const Value& right);
Value castValue(const Value& value, const RuntimeType& targetType);
