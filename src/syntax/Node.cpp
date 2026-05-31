#include "syntax/Node.hpp"

#include <stdexcept>

#include "syntax/Declarations.hpp"
#include "syntax/Expressions.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Visitor.hpp"

void Node::accept(Visitor& visitor) const {
   throw std::runtime_error("unsupported syntax node in visitor");
}

#define DJM_DEFINE_ACCEPT(Type) \
   void Type::accept(Visitor& visitor) const { visitor.visit(*this); }

DJM_DEFINE_ACCEPT(Program)
DJM_DEFINE_ACCEPT(FunctionDeclaration)
DJM_DEFINE_ACCEPT(GlobalConstantDeclaration)
DJM_DEFINE_ACCEPT(ImportDeclaration)

DJM_DEFINE_ACCEPT(BlockStatement)
DJM_DEFINE_ACCEPT(ExpressionStatement)
DJM_DEFINE_ACCEPT(VariableDeclarationStatement)
DJM_DEFINE_ACCEPT(ReturnStatement)
DJM_DEFINE_ACCEPT(BreakStatement)
DJM_DEFINE_ACCEPT(ContinueStatement)
DJM_DEFINE_ACCEPT(IfStatement)
DJM_DEFINE_ACCEPT(WhileStatement)
DJM_DEFINE_ACCEPT(ForStatement)
DJM_DEFINE_ACCEPT(AssignmentStatement)

DJM_DEFINE_ACCEPT(IntLiteralExpression)
DJM_DEFINE_ACCEPT(FloatLiteralExpression)
DJM_DEFINE_ACCEPT(BoolLiteralExpression)
DJM_DEFINE_ACCEPT(CharLiteralExpression)
DJM_DEFINE_ACCEPT(StringLiteralExpression)
DJM_DEFINE_ACCEPT(ListLiteralExpression)
DJM_DEFINE_ACCEPT(IdentifierExpression)
DJM_DEFINE_ACCEPT(ThisExpression)
DJM_DEFINE_ACCEPT(AddExpression)
DJM_DEFINE_ACCEPT(SubtractExpression)
DJM_DEFINE_ACCEPT(MultiplyExpression)
DJM_DEFINE_ACCEPT(DivideExpression)
DJM_DEFINE_ACCEPT(PowerExpression)
DJM_DEFINE_ACCEPT(LogicalOrExpression)
DJM_DEFINE_ACCEPT(LogicalAndExpression)
DJM_DEFINE_ACCEPT(EqualExpression)
DJM_DEFINE_ACCEPT(NotEqualExpression)
DJM_DEFINE_ACCEPT(ContainsExpression)
DJM_DEFINE_ACCEPT(LessExpression)
DJM_DEFINE_ACCEPT(LessEqualExpression)
DJM_DEFINE_ACCEPT(GreaterExpression)
DJM_DEFINE_ACCEPT(GreaterEqualExpression)
DJM_DEFINE_ACCEPT(MapExpression)
DJM_DEFINE_ACCEPT(FilterExpression)
DJM_DEFINE_ACCEPT(GroupExpression)
DJM_DEFINE_ACCEPT(NegateExpression)
DJM_DEFINE_ACCEPT(NotExpression)
DJM_DEFINE_ACCEPT(CountExpression)
DJM_DEFINE_ACCEPT(ReverseExpression)
DJM_DEFINE_ACCEPT(FlattenExpression)
DJM_DEFINE_ACCEPT(CastExpression)
DJM_DEFINE_ACCEPT(MemberAccessExpression)
DJM_DEFINE_ACCEPT(CallExpression)
DJM_DEFINE_ACCEPT(IndexExpression)
DJM_DEFINE_ACCEPT(SliceExpression)

#undef DJM_DEFINE_ACCEPT
