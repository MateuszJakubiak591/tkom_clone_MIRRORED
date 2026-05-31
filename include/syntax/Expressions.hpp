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


////////////////////////////////////////////////////////////////////////////////////////////////////////

class IntLiteralExpression final : public Expression {
public:
   IntLiteralExpression(SourceLocation location, int64_t value)
      : Expression(std::move(location)),
        value_(value) {}

   int64_t value() const {
      return value_;
   }

   void accept(Visitor& visitor) const override;

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

   void accept(Visitor& visitor) const override;

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

   void accept(Visitor& visitor) const override;

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

   void accept(Visitor& visitor) const override;

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

   void accept(Visitor& visitor) const override;

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

   void accept(Visitor& visitor) const override;

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

   void accept(Visitor& visitor) const override;

private:
   std::string name_;
};

class ThisExpression final : public Expression {
public:
   explicit ThisExpression(SourceLocation location)
      : Expression(std::move(location)) {}

   void accept(Visitor& visitor) const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
expression = logical_or_expr ;

logical_or_expr = logical_and_expr ,
                  { "||" , logical_and_expr } ;

logical_and_expr
                = equality_expr ,
                  { "&&" , equality_expr } ;

equality_expr   = relational_expr ,
                  [ ( "==" | "!=" | "contains" ) , relational_expr ] ;

relational_expr = additive_expr ,
                  [ ( "<" | "<=" | ">" | ">=" ) , additive_expr ] ;

additive_expr   = multiplicative_expr ,
                  { ( "+" | "-" ) , multiplicative_expr } ;

multiplicative_expr
                = list_operator_expr ,
                  { ( "*" | "/" ) , list_operator_expr } ;

list_operator_expr
                = map_expr ;

map_expr        = filter_expr ,
                  { "|>" , filter_expr } ;

filter_expr     = group_expr ,
                  { "?" , group_expr } ;

group_expr      = cast_expr ,
                  { "%" , cast_expr } ;

cast_expr       = unary_expr ,
                  { "as" , value_type } ;

unary_expr      = unary_op , unary_expr
                | power_expr ;

power_expr      = postfix_expr ,
                  [ "^" , unary_expr ] ;

unary_op        = "-"
                | "!"
                | "count"
                | "reverse"
                | "flatten" ;


primary_expr    = literal
                | identifier
                | "(" , expression , ")"
                | "this" ;
*/
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
   void accept(Visitor& visitor) const override;
};

class SubtractExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class MultiplyExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class DivideExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class PowerExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class LogicalOrExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class LogicalAndExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class EqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class NotEqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class ContainsExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class LessExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class LessEqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class GreaterExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class GreaterEqualExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class MapExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class FilterExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

class GroupExpression final : public BinaryExpression {
public:
   using BinaryExpression::BinaryExpression;
   void accept(Visitor& visitor) const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
unary_op        = "-"
                | "!"
                | "count"
                | "reverse"
                | "flatten" ;
*/

class UnaryExpression : public Expression {
public:
   UnaryExpression(SourceLocation location, ExprPtr operand)
      : Expression(std::move(location)),
        operand_(std::move(operand)) {}

   const Expression& operand() const {
      return *operand_;
   }

private:
   ExprPtr operand_;
};

class NegateExpression final : public UnaryExpression {
public:
   using UnaryExpression::UnaryExpression;
   void accept(Visitor& visitor) const override;
};

class NotExpression final : public UnaryExpression {
public:
   using UnaryExpression::UnaryExpression;
   void accept(Visitor& visitor) const override;
};

class CountExpression final : public UnaryExpression {
public:
   using UnaryExpression::UnaryExpression;
   void accept(Visitor& visitor) const override;
};

class ReverseExpression final : public UnaryExpression {
public:
   using UnaryExpression::UnaryExpression;
   void accept(Visitor& visitor) const override;
};

class FlattenExpression final : public UnaryExpression {
public:
   using UnaryExpression::UnaryExpression;
   void accept(Visitor& visitor) const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

class CastExpression final : public Expression {
public:
   CastExpression(SourceLocation location, ExprPtr expression, std::unique_ptr<TypeNode> targetType)
      : Expression(std::move(location)),
        expression_(std::move(expression)),
        targetType_(std::move(targetType)) {}

   const Expression& expression() const {
      return *expression_;
   }

   const TypeNode& targetType() const {
      return *targetType_;
   }

   void accept(Visitor& visitor) const override;

private:
   ExprPtr expression_;
   std::unique_ptr<TypeNode> targetType_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////

class MemberAccessExpression final : public Expression {
public:
   MemberAccessExpression(
      SourceLocation location,
      ExprPtr object,
      std::string memberName
   )
      : Expression(std::move(location)),
        object_(std::move(object)),
        memberName_(std::move(memberName)) {}

   const Expression& object() const {
      return *object_;
   }

   const std::string& memberName() const {
      return memberName_;
   }

   void accept(Visitor& visitor) const override;

private:
   ExprPtr object_;
   std::string memberName_;
};

class CallExpression final : public Expression {
public:
   CallExpression(SourceLocation location, ExprPtr callee, std::vector<ExprPtr> arguments)
      : Expression(std::move(location)),
        callee_(std::move(callee)),
        arguments_(std::move(arguments)) {}

   const Expression& callee() const {
      return *callee_;
   }

   const std::vector<ExprPtr>& arguments() const {
      return arguments_;
   }

   void accept(Visitor& visitor) const override;

private:
   ExprPtr callee_;
   std::vector<ExprPtr> arguments_;
};

class IndexExpression final : public Expression {
public:
   IndexExpression(SourceLocation location, ExprPtr object, ExprPtr index)
      : Expression(std::move(location)),
        object_(std::move(object)),
        index_(std::move(index)) {}

   const Expression& object() const {
      return *object_;
   }

   const Expression& index() const {
      return *index_;
   }

   void accept(Visitor& visitor) const override;

private:
   ExprPtr object_;
   ExprPtr index_;
};

class SliceExpression final : public Expression {
public:
   SliceExpression(
      SourceLocation location,
      ExprPtr object,
      ExprPtr start,
      ExprPtr end
   )
      : Expression(std::move(location)),
        object_(std::move(object)),
        start_(std::move(start)),
        end_(std::move(end)) {}

   const Expression& object() const {
      return *object_;
   }

   const Expression* start() const {
      return start_.get();
   }

   const Expression* end() const {
      return end_.get();
   }

   void accept(Visitor& visitor) const override;

private:
   ExprPtr object_;
   ExprPtr start_; // dopuszczalne nullptr dla xs[:2]
   ExprPtr end_;   // analogicznie nullptr dla xs[2:]
};

