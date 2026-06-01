#include "interpreter/RuntimeValue.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
struct IntegerParseResult {
   uint64_t checked = 0;
   uint64_t wrapped = 0;
   bool outOfRange = false;
};

IntegerParseResult parseUnsignedDigits(const std::string& text, std::size_t start, uint64_t limit) {
   IntegerParseResult result;

   if (start == text.size()) {
      throw RuntimeValueInvalidStringCast("missing digits");
   }

   for (std::size_t i = start; i < text.size(); ++i) {
      if (text[i] < '0' || text[i] > '9') {
         throw RuntimeValueInvalidStringCast("invalid digit");
      }

      const uint64_t digit = static_cast<uint64_t>(text[i] - '0');
      result.wrapped = result.wrapped * 10 + digit;

      if (!result.outOfRange) {
         if (result.checked > (limit - digit) / 10) {
            result.outOfRange = true;
         } else {
            result.checked = result.checked * 10 + digit;
         }
      }
   }

   return result;
}

int64_t int64FromWrappedBits(uint64_t bits) {
   constexpr uint64_t signBit = uint64_t{1} << 63;

   if ((bits & signBit) == 0) {
      return static_cast<int64_t>(bits);
   }

   const uint64_t magnitude = 0 - bits;
   if (magnitude == signBit) {
      return std::numeric_limits<int64_t>::min();
   }

   return -static_cast<int64_t>(magnitude);
}

char charFromWrappedBits(uint64_t bits) {
   return static_cast<char>(static_cast<unsigned char>(bits));
}

uint64_t wrappingIntegralPart(double value) {
   long double wrapped = std::fmod(
      std::trunc(static_cast<long double>(value)),
      std::ldexp(1.0L, 64)
   );

   if (wrapped < 0.0L) {
      wrapped += std::ldexp(1.0L, 64);
   }

   return static_cast<uint64_t>(wrapped);
}

}

int64_t stringToInt(const std::string& text) {
   bool negative = false;
   std::size_t start = 0;

   if (!text.empty() && text[0] == '-') {
      negative = true;
      start = 1;
   }

   const uint64_t maxMagnitude = static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1;
   IntegerParseResult parsed;
   try {
      parsed = parseUnsignedDigits(text, start, negative ? maxMagnitude : maxMagnitude - 1);
   } catch (const RuntimeValueInvalidStringCast&) {
      throw RuntimeValueInvalidStringCast("cannot cast string '" + text + "' to int");
   }

   const uint64_t wrappedBits = negative ? 0 - parsed.wrapped : parsed.wrapped;
   if (parsed.outOfRange) {
      throw RuntimeValueOutOfRange(
         "string '" + text + "' is outside int range",
         Value::intValue(int64FromWrappedBits(wrappedBits))
      );
   }

   if (negative) {
      if (parsed.checked == maxMagnitude) {
         return std::numeric_limits<int64_t>::min();
      }
      return -static_cast<int64_t>(parsed.checked);
   }

   return static_cast<int64_t>(parsed.checked);
}

uint64_t stringToUint(const std::string& text) {
   IntegerParseResult parsed;
   try {
      parsed = parseUnsignedDigits(text, 0, std::numeric_limits<uint64_t>::max());
   } catch (const RuntimeValueInvalidStringCast&) {
      throw RuntimeValueInvalidStringCast("cannot cast string '" + text + "' to uint");
   }

   if (parsed.outOfRange) {
      throw RuntimeValueOutOfRange(
         "string '" + text + "' is outside uint range",
         Value::uintValue(parsed.wrapped)
      );
   }

   return parsed.checked;
}

double stringToFloat(const std::string& text) {
   double result = 0.0;
   std::size_t i = 0;
   bool negative = false;

   if (!text.empty() && text[0] == '-') {
      negative = true;
      i = 1;
   }

   if (i == text.size()) {
      throw RuntimeValueInvalidStringCast("cannot cast string '" + text + "' to float");
   }

   bool hasDigits = false;
   while (i < text.size() && text[i] != '.') {
      if (text[i] < '0' || text[i] > '9') {
         throw RuntimeValueInvalidStringCast("cannot cast string '" + text + "' to float");
      }

      hasDigits = true;
      result = result * 10.0 + (text[i] - '0');
      ++i;
   }

   if (i < text.size() && text[i] == '.') {
      ++i;
      double weight = 0.1;
      while (i < text.size()) {
         if (text[i] < '0' || text[i] > '9') {
            throw RuntimeValueInvalidStringCast("cannot cast string '" + text + "' to float");
         }

         hasDigits = true;
         result += (text[i] - '0') * weight;
         weight /= 10.0;
         ++i;
      }
   }

   if (!hasDigits) {
      throw RuntimeValueInvalidStringCast("cannot cast string '" + text + "' to float");
   }

   return negative ? -result : result;
}

RuntimeType::RuntimeType(Kind kind, std::shared_ptr<RuntimeType> elementType)
   : kind_(kind),
     elementType_(std::move(elementType)) {}

RuntimeType RuntimeType::voidType() { return RuntimeType{Kind::Void}; }
RuntimeType RuntimeType::intType() { return RuntimeType{Kind::Int}; }
RuntimeType RuntimeType::uintType() { return RuntimeType{Kind::Uint}; }
RuntimeType RuntimeType::floatType() { return RuntimeType{Kind::Float}; }
RuntimeType RuntimeType::boolType() { return RuntimeType{Kind::Bool}; }
RuntimeType RuntimeType::charType() { return RuntimeType{Kind::Char}; }
RuntimeType RuntimeType::stringType() { return RuntimeType{Kind::String}; }

RuntimeType RuntimeType::listOf(RuntimeType elementType) {
   return RuntimeType{Kind::List, std::make_shared<RuntimeType>(std::move(elementType))};
}

RuntimeType::Kind RuntimeType::kind() const {
   return kind_;
}

const RuntimeType& RuntimeType::elementType() const {
   if (elementType_ == nullptr) {
      throw std::logic_error("runtime type does not have an element type");
   }

   return *elementType_;
}

bool RuntimeType::operator==(const RuntimeType& other) const {
   if (kind_ != other.kind_) {
      return false;
   }

   if (kind_ != Kind::List) {
      return true;
   }

   return elementType_ != nullptr && other.elementType_ != nullptr && *elementType_ == *other.elementType_;
}

bool RuntimeType::operator!=(const RuntimeType& other) const {
   return !(*this == other);
}

std::string RuntimeType::toString() const {
   switch (kind_) {
      case Kind::Void: return "void";
      case Kind::Int: return "int";
      case Kind::Uint: return "uint";
      case Kind::Float: return "float";
      case Kind::Bool: return "bool";
      case Kind::Char: return "char";
      case Kind::String: return "string";
      case Kind::List:
         return "list<" + elementType_->toString() + ">";
   }

   return "unknown";
}

RuntimeType runtimeTypeFromNode(const TypeNode& node) {
   if (dynamic_cast<const VoidTypeNode*>(&node)) return RuntimeType::voidType();
   if (dynamic_cast<const IntTypeNode*>(&node)) return RuntimeType::intType();
   if (dynamic_cast<const UintTypeNode*>(&node)) return RuntimeType::uintType();
   if (dynamic_cast<const FloatTypeNode*>(&node)) return RuntimeType::floatType();
   if (dynamic_cast<const BoolTypeNode*>(&node)) return RuntimeType::boolType();
   if (dynamic_cast<const CharTypeNode*>(&node)) return RuntimeType::charType();
   if (dynamic_cast<const StringTypeNode*>(&node)) return RuntimeType::stringType();

   if (const auto* list = dynamic_cast<const ListTypeNode*>(&node)) {
      return RuntimeType::listOf(runtimeTypeFromNode(list->elementType()));
   }

   throw std::runtime_error("unsupported type node");
}

Value::Value(RuntimeType type, Data data)
   : type_(std::move(type)),
     data_(std::move(data)) {}

Value Value::voidValue() { return Value{RuntimeType::voidType(), std::monostate{}}; }
Value Value::intValue(int64_t value) { return Value{RuntimeType::intType(), value}; }
Value Value::uintValue(uint64_t value) { return Value{RuntimeType::uintType(), value}; }
Value Value::floatValue(double value) { return Value{RuntimeType::floatType(), value}; }
Value Value::boolValue(bool value) { return Value{RuntimeType::boolType(), value}; }
Value Value::charValue(char value) { return Value{RuntimeType::charType(), value}; }
Value Value::stringValue(std::string value) { return Value{RuntimeType::stringType(), std::move(value)}; }

Value Value::listValue(RuntimeType elementType, ValueList elements) {
   return Value{RuntimeType::listOf(std::move(elementType)), std::move(elements)};
}

const RuntimeType& Value::type() const {
   return type_;
}

const Value::Data& Value::data() const {
   return data_;
}

std::string Value::toString() const {
   std::ostringstream oss;

   switch (type_.kind()) {
      case RuntimeType::Kind::Void:
         return "void";
      case RuntimeType::Kind::Int:
         return std::to_string(std::get<int64_t>(data_));
      case RuntimeType::Kind::Uint:
         return std::to_string(std::get<uint64_t>(data_));
      case RuntimeType::Kind::Float:
         oss << std::get<double>(data_);
         return oss.str();
      case RuntimeType::Kind::Bool:
         return std::get<bool>(data_) ? "true" : "false";
      case RuntimeType::Kind::Char:
         return std::string(1, std::get<char>(data_));
      case RuntimeType::Kind::String:
         return std::get<std::string>(data_);
      case RuntimeType::Kind::List: {
         oss << "[";
         const auto& elements = std::get<ValueList>(data_);
         for (std::size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) {
               oss << ", ";
            }
            oss << elements[i].toString();
         }
         oss << "]";
         return oss.str();
      }
   }

   return "";
}

ValueObject::ValueObject(Value value, bool isMutable)
   : value_(std::move(value)),
     isMutable_(isMutable) {}

const RuntimeType& ValueObject::type() const { return value_.type(); }
const Value& ValueObject::value() const { return value_; }
bool ValueObject::isMutable() const { return isMutable_; }

void ValueObject::assign(Value value) {
   value_ = std::move(value);
}

RuntimeValueInvalidStringCast::RuntimeValueInvalidStringCast(const std::string& message)
   : std::runtime_error(message) {}

RuntimeValueOutOfRange::RuntimeValueOutOfRange(const std::string& message, Value wrappedValue)
   : std::runtime_error(message),
     wrappedValue_(std::move(wrappedValue)) {}

const Value& RuntimeValueOutOfRange::wrappedValue() const {
   return wrappedValue_;
}

Value defaultValueFor(const RuntimeType& type) {
   switch (type.kind()) {
      case RuntimeType::Kind::Void: return Value::voidValue();
      case RuntimeType::Kind::Int: return Value::intValue(0);
      case RuntimeType::Kind::Uint: return Value::uintValue(0);
      case RuntimeType::Kind::Float: return Value::floatValue(0.0);
      case RuntimeType::Kind::Bool: return Value::boolValue(false);
      case RuntimeType::Kind::Char: return Value::charValue('\0');
      case RuntimeType::Kind::String: return Value::stringValue("");
      case RuntimeType::Kind::List: return Value::listValue(type.elementType(), {});
   }

   return Value::voidValue();
}

Value cloneValue(const Value& value) {
   return value;
}

bool valuesEqual(const Value& left, const Value& right) {
   if (left.type() != right.type()) {
      return false;
   }

   switch (left.type().kind()) {
      case RuntimeType::Kind::Void: return true;
      case RuntimeType::Kind::Int: return std::get<int64_t>(left.data()) == std::get<int64_t>(right.data());
      case RuntimeType::Kind::Uint: return std::get<uint64_t>(left.data()) == std::get<uint64_t>(right.data());
      case RuntimeType::Kind::Float: return std::get<double>(left.data()) == std::get<double>(right.data());
      case RuntimeType::Kind::Bool: return std::get<bool>(left.data()) == std::get<bool>(right.data());
      case RuntimeType::Kind::Char: return std::get<char>(left.data()) == std::get<char>(right.data());
      case RuntimeType::Kind::String: return std::get<std::string>(left.data()) == std::get<std::string>(right.data());
      case RuntimeType::Kind::List: {
         const auto& l = std::get<ValueList>(left.data());
         const auto& r = std::get<ValueList>(right.data());
         return l.size() == r.size() && std::equal(l.begin(), l.end(), r.begin(), valuesEqual);
      }
   }

   return false;
}

int compareValues(const Value& left, const Value& right) {
   if (left.type() != right.type()) {
      throw std::runtime_error("cannot compare " + left.type().toString() + " with " + right.type().toString());
   }

   switch (left.type().kind()) {
      case RuntimeType::Kind::Int:
         return (std::get<int64_t>(left.data()) > std::get<int64_t>(right.data())) -
                (std::get<int64_t>(left.data()) < std::get<int64_t>(right.data()));
      case RuntimeType::Kind::Uint:
         return (std::get<uint64_t>(left.data()) > std::get<uint64_t>(right.data())) -
                (std::get<uint64_t>(left.data()) < std::get<uint64_t>(right.data()));
      case RuntimeType::Kind::Float:
         return (std::get<double>(left.data()) > std::get<double>(right.data())) -
                (std::get<double>(left.data()) < std::get<double>(right.data()));
      case RuntimeType::Kind::Bool:
         return (std::get<bool>(left.data()) > std::get<bool>(right.data())) -
                (std::get<bool>(left.data()) < std::get<bool>(right.data()));
      case RuntimeType::Kind::Char:
         return (std::get<char>(left.data()) > std::get<char>(right.data())) -
                (std::get<char>(left.data()) < std::get<char>(right.data()));
      case RuntimeType::Kind::String:
         return std::get<std::string>(left.data()).compare(std::get<std::string>(right.data()));
      case RuntimeType::Kind::List: {
         const auto& l = std::get<ValueList>(left.data());
         const auto& r = std::get<ValueList>(right.data());
         const std::size_t count = std::min(l.size(), r.size());
         for (std::size_t i = 0; i < count; ++i) {
            int cmp = compareValues(l[i], r[i]);
            if (cmp != 0) {
               return cmp;
            }
         }
         return (l.size() > r.size()) - (l.size() < r.size());
      }
      case RuntimeType::Kind::Void:
         return 0;
   }

   return 0;
}

Value castValue(const Value& value, const RuntimeType& targetType) {
   if (value.type() == targetType) {
      return cloneValue(value);
   }

   if (targetType.kind() == RuntimeType::Kind::String) {
      return Value::stringValue(value.toString());
   }

   switch (value.type().kind()) {
      case RuntimeType::Kind::Int: {
         auto v = std::get<int64_t>(value.data());
         if (targetType.kind() == RuntimeType::Kind::Uint) {
            Value result = Value::uintValue(static_cast<uint64_t>(v));
            if (v < 0) {
               throw RuntimeValueOutOfRange("int value is outside uint range", std::move(result));
            }
            return result;
         }
         if (targetType.kind() == RuntimeType::Kind::Float) return Value::floatValue(static_cast<double>(v));
         if (targetType.kind() == RuntimeType::Kind::Bool) return Value::boolValue(v > 0);
         if (targetType.kind() == RuntimeType::Kind::Char) {
            Value result = Value::charValue(charFromWrappedBits(static_cast<uint64_t>(v)));
            if (v < 0 || v > std::numeric_limits<unsigned char>::max()) {
               throw RuntimeValueOutOfRange("int value is outside char range", std::move(result));
            }
            return result;
         }
         break;
      }
      case RuntimeType::Kind::Uint: {
         auto v = std::get<uint64_t>(value.data());
         if (targetType.kind() == RuntimeType::Kind::Int) {
            Value result = Value::intValue(int64FromWrappedBits(v));
            if (v > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
               throw RuntimeValueOutOfRange("uint value is outside int range", std::move(result));
            }
            return result;
         }
         if (targetType.kind() == RuntimeType::Kind::Float) return Value::floatValue(static_cast<double>(v));
         if (targetType.kind() == RuntimeType::Kind::Bool) return Value::boolValue(v != 0);
         if (targetType.kind() == RuntimeType::Kind::Char) {
            Value result = Value::charValue(charFromWrappedBits(v));
            if (v > std::numeric_limits<unsigned char>::max()) {
               throw RuntimeValueOutOfRange("uint value is outside char range", std::move(result));
            }
            return result;
         }
         break;
      }
      case RuntimeType::Kind::Float: {
         auto v = std::get<double>(value.data());
         if (targetType.kind() == RuntimeType::Kind::Int) {
            if (v < static_cast<double>(std::numeric_limits<int64_t>::min()) ||
                v > static_cast<double>(std::numeric_limits<int64_t>::max())) {
               Value result = Value::intValue(int64FromWrappedBits(wrappingIntegralPart(v)));
               throw RuntimeValueOutOfRange("float value is outside int range", std::move(result));
            }
            Value result = Value::intValue(static_cast<int64_t>(v));
            return result;
         }
         if (targetType.kind() == RuntimeType::Kind::Uint) {
            if (v < 0.0 || v > static_cast<double>(std::numeric_limits<uint64_t>::max())) {
               Value result = Value::uintValue(wrappingIntegralPart(v));
               throw RuntimeValueOutOfRange("float value is outside uint range", std::move(result));
            }
            Value result = Value::uintValue(static_cast<uint64_t>(v));
            return result;
         }
         if (targetType.kind() == RuntimeType::Kind::Bool) return Value::boolValue(v > 0.0);
         if (targetType.kind() == RuntimeType::Kind::Char) {
            if (v < 0.0 || v > static_cast<double>(std::numeric_limits<unsigned char>::max())) {
               Value result = Value::charValue(charFromWrappedBits(wrappingIntegralPart(v)));
               throw RuntimeValueOutOfRange("float value is outside char range", std::move(result));
            }
            Value result = Value::charValue(static_cast<char>(v));
            return result;
         }
         break;
      }
      case RuntimeType::Kind::Bool: {
         auto v = std::get<bool>(value.data());
         if (targetType.kind() == RuntimeType::Kind::Int) return Value::intValue(v ? 1 : 0);
         if (targetType.kind() == RuntimeType::Kind::Uint) return Value::uintValue(v ? 1 : 0);
         if (targetType.kind() == RuntimeType::Kind::Float) return Value::floatValue(v ? 1.0 : 0.0);
         if (targetType.kind() == RuntimeType::Kind::Char) return Value::charValue(v ? 1 : 0);
         break;
      }
      case RuntimeType::Kind::Char: {
         auto v = static_cast<unsigned char>(std::get<char>(value.data()));
         if (targetType.kind() == RuntimeType::Kind::Int) return Value::intValue(v);
         if (targetType.kind() == RuntimeType::Kind::Uint) return Value::uintValue(v);
         if (targetType.kind() == RuntimeType::Kind::Float) return Value::floatValue(v);
         if (targetType.kind() == RuntimeType::Kind::Bool) return Value::boolValue(v > 0);
         break;
      }
      case RuntimeType::Kind::String: {
         const auto& text = std::get<std::string>(value.data());
         if (targetType.kind() == RuntimeType::Kind::Int) return Value::intValue(stringToInt(text));
         if (targetType.kind() == RuntimeType::Kind::Uint) return Value::uintValue(stringToUint(text));
         if (targetType.kind() == RuntimeType::Kind::Float) return Value::floatValue(stringToFloat(text));
         break;
      }
      case RuntimeType::Kind::List: {
         if (targetType.kind() != RuntimeType::Kind::List) {
            break;
         }
         ValueList converted;
         std::string firstOutOfRangeError;
         for (const auto& element : std::get<ValueList>(value.data())) {
            try {
               converted.push_back(castValue(element, targetType.elementType()));
            } catch (const RuntimeValueOutOfRange& error) {
               converted.push_back(cloneValue(error.wrappedValue()));
               if (firstOutOfRangeError.empty()) {
                  firstOutOfRangeError = error.what();
               }
            }
         }
         if (!firstOutOfRangeError.empty()) {
            throw RuntimeValueOutOfRange(
               firstOutOfRangeError,
               Value::listValue(targetType.elementType(), std::move(converted))
            );
         }
         return Value::listValue(targetType.elementType(), std::move(converted));
      }
      default:
         break;
   }

   throw std::runtime_error("cannot cast " + value.type().toString() + " to " + targetType.toString());
}
