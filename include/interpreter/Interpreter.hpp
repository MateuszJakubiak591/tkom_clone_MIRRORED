#pragma once

#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

#include "diagnostics/ErrorHandler.hpp"
#include "interpreter/Environment.hpp"
#include "syntax/Visitor.hpp"

class Interpreter final : public Visitor {
public:
   explicit Interpreter(ErrorHandler* errorHandler = nullptr, std::ostream* output = nullptr);

   int interpret(const Program& program, const std::vector<std::string>& args = {});

   Value executeUserFunction(const FunctionDeclaration& declaration, const std::vector<Value>& args);

   void visit(const Program& node) override;
   void visit(const FunctionDeclaration& node) override;
   void visit(const GlobalConstantDeclaration& node) override;
   void visit(const ImportDeclaration& node) override;

   void visit(const BlockStatement& node) override;
   void visit(const ExpressionStatement& node) override;
   void visit(const VariableDeclarationStatement& node) override;
   void visit(const ReturnStatement& node) override;
   void visit(const BreakStatement& node) override;
   void visit(const ContinueStatement& node) override;
   void visit(const IfStatement& node) override;
   void visit(const WhileStatement& node) override;
   void visit(const ForStatement& node) override;
   void visit(const AssignmentStatement& node) override;

   void visit(const IntLiteralExpression& node) override;
   void visit(const FloatLiteralExpression& node) override;
   void visit(const BoolLiteralExpression& node) override;
   void visit(const CharLiteralExpression& node) override;
   void visit(const StringLiteralExpression& node) override;
   void visit(const ListLiteralExpression& node) override;
   void visit(const IdentifierExpression& node) override;
   void visit(const ThisExpression& node) override;
   void visit(const AddExpression& node) override;
   void visit(const SubtractExpression& node) override;
   void visit(const MultiplyExpression& node) override;
   void visit(const DivideExpression& node) override;
   void visit(const PowerExpression& node) override;
   void visit(const LogicalOrExpression& node) override;
   void visit(const LogicalAndExpression& node) override;
   void visit(const EqualExpression& node) override;
   void visit(const NotEqualExpression& node) override;
   void visit(const ContainsExpression& node) override;
   void visit(const LessExpression& node) override;
   void visit(const LessEqualExpression& node) override;
   void visit(const GreaterExpression& node) override;
   void visit(const GreaterEqualExpression& node) override;
   void visit(const MapExpression& node) override;
   void visit(const FilterExpression& node) override;
   void visit(const GroupExpression& node) override;
   void visit(const NegateExpression& node) override;
   void visit(const NotExpression& node) override;
   void visit(const CountExpression& node) override;
   void visit(const ReverseExpression& node) override;
   void visit(const FlattenExpression& node) override;
   void visit(const CastExpression& node) override;
   void visit(const MemberAccessExpression& node) override;
   void visit(const CallExpression& node) override;
   void visit(const IndexExpression& node) override;
   void visit(const SliceExpression& node) override;

private:
   struct ReturnSignal {
      Value value;
   };

   struct BreakSignal {};
   struct ContinueSignal {};

   Environment environment_;
   ErrorHandler* errorHandler_;
   std::ostream* output_;
   std::optional<Value> lastValue_;
   std::optional<Value> currentThis_;
   std::vector<std::string> programArgs_;

   Value evaluate(const Expression& expression);
   Value evaluateWithThis(const Expression& expression, const Value& thisValue);
   ValueRef resolveAssignable(const Expression& expression);
   Value evaluateBinaryNumeric(const BinaryExpression& node, char operation);
   Value evaluateComparison(const BinaryExpression& node, const std::string& operation);
   Value coerceForAssignment(Value value, const RuntimeType& targetType, const SourceLocation& location);
   bool asBool(const Value& value, const SourceLocation& location) const;
   void executeBlock(const BlockStatement& block, bool createScope);
   void reportRuntimeError(const RuntimeError& error);
};
