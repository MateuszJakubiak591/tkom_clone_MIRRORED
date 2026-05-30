#include "syntax/Node.hpp"

#include <stdexcept>

#include "syntax/Declarations.hpp"
#include "syntax/Expressions.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Visitor.hpp"

namespace {
template <typename T>
bool tryVisit(const Node& node, Visitor& visitor) {
   if (const auto* typed = dynamic_cast<const T*>(&node)) {
      visitor.visit(*typed);
      return true;
   }

   return false;
}
}

void Node::accept(Visitor& visitor) const {
   if (tryVisit<Program>(*this, visitor)) return;
   if (tryVisit<FunctionDeclaration>(*this, visitor)) return;
   if (tryVisit<GlobalConstantDeclaration>(*this, visitor)) return;
   if (tryVisit<ImportDeclaration>(*this, visitor)) return;

   if (tryVisit<BlockStatement>(*this, visitor)) return;
   if (tryVisit<ExpressionStatement>(*this, visitor)) return;
   if (tryVisit<VariableDeclarationStatement>(*this, visitor)) return;
   if (tryVisit<ReturnStatement>(*this, visitor)) return;
   if (tryVisit<BreakStatement>(*this, visitor)) return;
   if (tryVisit<ContinueStatement>(*this, visitor)) return;
   if (tryVisit<IfStatement>(*this, visitor)) return;
   if (tryVisit<WhileStatement>(*this, visitor)) return;
   if (tryVisit<ForStatement>(*this, visitor)) return;
   if (tryVisit<AssignmentStatement>(*this, visitor)) return;

   if (tryVisit<IntLiteralExpression>(*this, visitor)) return;
   if (tryVisit<FloatLiteralExpression>(*this, visitor)) return;
   if (tryVisit<BoolLiteralExpression>(*this, visitor)) return;
   if (tryVisit<CharLiteralExpression>(*this, visitor)) return;
   if (tryVisit<StringLiteralExpression>(*this, visitor)) return;
   if (tryVisit<ListLiteralExpression>(*this, visitor)) return;
   if (tryVisit<IdentifierExpression>(*this, visitor)) return;
   if (tryVisit<ThisExpression>(*this, visitor)) return;
   if (tryVisit<AddExpression>(*this, visitor)) return;
   if (tryVisit<SubtractExpression>(*this, visitor)) return;
   if (tryVisit<MultiplyExpression>(*this, visitor)) return;
   if (tryVisit<DivideExpression>(*this, visitor)) return;
   if (tryVisit<PowerExpression>(*this, visitor)) return;
   if (tryVisit<LogicalOrExpression>(*this, visitor)) return;
   if (tryVisit<LogicalAndExpression>(*this, visitor)) return;
   if (tryVisit<EqualExpression>(*this, visitor)) return;
   if (tryVisit<NotEqualExpression>(*this, visitor)) return;
   if (tryVisit<ContainsExpression>(*this, visitor)) return;
   if (tryVisit<LessExpression>(*this, visitor)) return;
   if (tryVisit<LessEqualExpression>(*this, visitor)) return;
   if (tryVisit<GreaterExpression>(*this, visitor)) return;
   if (tryVisit<GreaterEqualExpression>(*this, visitor)) return;
   if (tryVisit<MapExpression>(*this, visitor)) return;
   if (tryVisit<FilterExpression>(*this, visitor)) return;
   if (tryVisit<GroupExpression>(*this, visitor)) return;
   if (tryVisit<NegateExpression>(*this, visitor)) return;
   if (tryVisit<NotExpression>(*this, visitor)) return;
   if (tryVisit<CountExpression>(*this, visitor)) return;
   if (tryVisit<ReverseExpression>(*this, visitor)) return;
   if (tryVisit<FlattenExpression>(*this, visitor)) return;
   if (tryVisit<CastExpression>(*this, visitor)) return;
   if (tryVisit<MemberAccessExpression>(*this, visitor)) return;
   if (tryVisit<CallExpression>(*this, visitor)) return;
   if (tryVisit<IndexExpression>(*this, visitor)) return;
   if (tryVisit<SliceExpression>(*this, visitor)) return;

   throw std::runtime_error("unsupported syntax node in visitor");
}
