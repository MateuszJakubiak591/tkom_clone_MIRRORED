#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "syntax/Type.hpp"

class RuntimeType {
public:
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
   static RuntimeType listOf(RuntimeType elementType);

   Kind kind() const;
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
   static Value listValue(RuntimeType elementType, ValueList elements);

   const RuntimeType& type() const;
   const Data& data() const;

   std::string toString() const;

private:
   Value(RuntimeType type, Data data);

   RuntimeType type_;
   Data data_;
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
