#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "syntax/Node.hpp"
#include "syntax/Type.hpp"

class Expression : public Node {
public:
   using Node::Node;
   ~Expression() override = default;
};

using ExprPtr = std::unique_ptr<Expression>;

class IntLiteralExpression final : public Expression {
public:
   IntLiteralExpression(SourceLocation location, int64_t value)
      : Expression(std::move(location)),
        value_(value) {}

   int64_t value() const {
      return value_;
   }

private:
   int64_t value_;
};

class FloatLiteralExpression final : public Expression {
public:
   FloatLiteralExpression(SourceLocation location, double value)
      : Expression(std::move(location)),
        value_(value) {}

   double value() const {
      return value_;
   }

private:
   double value_;
};

class BoolLiteralExpression final : public Expression {
public:
   BoolLiteralExpression(SourceLocation location, bool value)
      : Expression(std::move(location)),
        value_(value) {}

   bool value() const {
      return value_;
   }

private:
   bool value_;
};

class CharLiteralExpression final : public Expression {
public:
   CharLiteralExpression(SourceLocation location, char value)
      : Expression(std::move(location)),
        value_(value) {}

   char value() const {
      return value_;
   }

private:
   char value_;
};

class StringLiteralExpression final : public Expression {
public:
   StringLiteralExpression(SourceLocation location, std::string value)
      : Expression(std::move(location)),
        value_(std::move(value)) {}

   const std::string& value() const {
      return value_;
   }

private:
   std::string value_;
};

class ListLiteralExpression final : public Expression {
public:
   ListLiteralExpression(SourceLocation location, std::vector<ExprPtr> elements)
      : Expression(std::move(location)),
        elements_(std::move(elements)) {}

   const std::vector<ExprPtr>& elements() const {
      return elements_;
   }

private:
   std::vector<ExprPtr> elements_;
};

class IdentifierExpression final : public Expression {
public:
   IdentifierExpression(SourceLocation location, std::string name)
      : Expression(std::move(location)),
        name_(std::move(name)) {}

   const std::string& name() const {
      return name_;
   }

private:
   std::string name_;
};

class ThisExpression final : public Expression {
public:
   explicit ThisExpression(SourceLocation location)
      : Expression(std::move(location)) {}
};

class BinaryExpression : public Expression {
public:
   BinaryExpression(SourceLocation location, ExprPtr left, ExprPtr right)
      : Expression(std::move(location)),
        left_(std::move(left)),
        right_(std::move(right)) {}

   const Expression& left() const {
      return *left_;
   }

   const Expression& right() const {
      return *right_;
   }

private:
   ExprPtr left_;
   ExprPtr right_;
};

class AddExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class SubtractExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class MultiplyExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class DivideExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class PowerExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class LogicalOrExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class LogicalAndExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class EqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class NotEqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class ContainsExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class LessExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class LessEqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class GreaterExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class GreaterEqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class MapExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class FilterExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};

class GroupExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
};s