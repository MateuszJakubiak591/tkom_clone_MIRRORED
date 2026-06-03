#include "interpreter/Interpreter.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <limits>
#include <system_error>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "source/FileSource.hpp"
#include "syntax/Declarations.hpp"
#include "syntax/Expressions.hpp"
#include "syntax/Statements.hpp"

namespace {
bool isNumeric(const RuntimeType& type) {
   return type.kind() == RuntimeType::Kind::Int ||
          type.kind() == RuntimeType::Kind::Uint ||
          type.kind() == RuntimeType::Kind::Float ||
          type.kind() == RuntimeType::Kind::Char;
}

int64_t toSignedInteger(const Value& value) {
   switch (value.type().kind()) {
      case RuntimeType::Kind::Int: return std::get<int64_t>(value.data());
      case RuntimeType::Kind::Uint: return static_cast<int64_t>(std::get<uint64_t>(value.data()));
      case RuntimeType::Kind::Char: return static_cast<unsigned char>(std::get<char>(value.data()));
      default: return 0;
   }
}

double toDouble(const Value& value) {
   switch (value.type().kind()) {
      case RuntimeType::Kind::Float: return std::get<double>(value.data());
      case RuntimeType::Kind::Int: return static_cast<double>(std::get<int64_t>(value.data()));
      case RuntimeType::Kind::Uint: return static_cast<double>(std::get<uint64_t>(value.data()));
      case RuntimeType::Kind::Char: return static_cast<unsigned char>(std::get<char>(value.data()));
      default: return 0.0;
   }
}

Value makeNumericResult(const RuntimeType& type, double floatValue, int64_t intValue) {
   switch (type.kind()) {
      case RuntimeType::Kind::Int: return Value::intValue(intValue);
      case RuntimeType::Kind::Uint: return Value::uintValue(static_cast<uint64_t>(intValue));
      case RuntimeType::Kind::Float: return Value::floatValue(floatValue);
      case RuntimeType::Kind::Char: return Value::charValue(static_cast<char>(intValue));
      default: break;
   }

   return Value::voidValue();
}

void requireSameListOperands(
   const Value& left,
   const Value& right,
   const std::string& operation,
   const SourceLocation& location
) {
   if (left.type().kind() != RuntimeType::Kind::List ||
       right.type().kind() != RuntimeType::Kind::List ||
       left.type() != right.type()) {
      throw RuntimeError(
         "operator " + operation + " requires lists of the same type",
         location
      );
   }
}
}

Interpreter::Interpreter(ErrorHandler* errorHandler, std::ostream* output, std::string mainFilePath)
   : errorHandler_(errorHandler == nullptr ? &nullErrorHandler_ : errorHandler),
     output_(output == nullptr ? &std::cout : output) {
   if (mainFilePath.empty()) {
      importRoot_ = std::filesystem::current_path();
   } else {
      importRoot_ = std::filesystem::absolute(std::filesystem::path(mainFilePath)).parent_path();
   }
}

int Interpreter::interpret(const Program& program, const std::vector<std::string>& args) {
   programArgs_ = args;

   try {
      program.accept(*this);
      if (!lastValue_) {
         return 0;
      }
      return static_cast<int>(toSignedInteger(*lastValue_));
   } catch (const RuntimeError& error) {
      reportRuntimeError(error);
      return 1;
   }
}

Value Interpreter::executeUserFunction(const FunctionDeclaration& declaration, const std::vector<Value>& args) {
   environment_.pushCallContext();

   try {
      const auto& parameters = declaration.parameters();
      for (std::size_t i = 0; i < parameters.size(); ++i) {
         RuntimeType paramType = runtimeTypeFromNode(*parameters[i].type);
         Value value = coerceForParameter(cloneValue(args[i]), paramType, parameters[i].name, parameters[i].location);
         environment_.defineVariable(
            parameters[i].name,
            std::make_shared<ValueObject>(std::move(value), true),
            parameters[i].location
         );
      }

      declaration.body().accept(*this);

      RuntimeType returnType = runtimeTypeFromNode(declaration.returnType());
      environment_.popCallContext();
      return defaultValueFor(returnType);
   } catch (const ReturnSignal& signal) {
      RuntimeType returnType = runtimeTypeFromNode(declaration.returnType());
      Value value = coerceForAssignment(signal.value, returnType, declaration.location());
      environment_.popCallContext();
      return value;
   } catch (...) {
      environment_.popCallContext();
      throw;
   }
}

void Interpreter::visit(const Program& node) {
   environment_.addBuiltins();
   loadImports(node);

   for (const auto& function : node.functionDeclarations()) {
      environment_.addFunction(*function);
   }

   for (const auto& declaration : node.globalConstantDeclarations()) {
      declaration->accept(*this);
   }

   Callable& main = environment_.findFunction("main", node.location());
   const FunctionDeclaration* mainDeclaration = nullptr;
   for (const auto& function : node.functionDeclarations()) {
      if (function->name() == "main") {
         mainDeclaration = function.get();
         break;
      }
   }

   std::vector<Value> args;

   bool shouldExecuteMainWithoutParameters = false;

   if (main.arity() == 1 && mainDeclaration != nullptr && mainSignatureIsValid(*mainDeclaration)) {
      ValueList values;
      for (const auto& arg : programArgs_) {
         values.push_back(Value::stringValue(arg));
      }
      args.push_back(Value::listValue(RuntimeType::stringType(), std::move(values)));
   } else if (main.arity() != 0) {
      reportRuntimeError(RuntimeError(
         "main must take zero arguments or list<string>; continuing as if main had no parameters",
         node.location()
      ));
      shouldExecuteMainWithoutParameters = true;
   }

   if (shouldExecuteMainWithoutParameters && mainDeclaration != nullptr) {
      lastValue_ = executeMainWithoutParameters(*mainDeclaration);
      return;
   } else if (shouldExecuteMainWithoutParameters) {
      throw RuntimeError("function not found: main", node.location());
   }

   lastValue_ = main.call(*this, args, node.location());
}

void Interpreter::visit(const FunctionDeclaration&) {}

void Interpreter::visit(const GlobalConstantDeclaration& node) {
   RuntimeType type = runtimeTypeFromNode(node.type());
   Value value = node.initializer() == nullptr
      ? defaultValueFor(type)
      : coerceForAssignment(evaluate(*node.initializer()), type, node.location());

   for (const auto& name : node.names()) {
      environment_.defineVariable(
         name.name,
         std::make_shared<ValueObject>(cloneValue(value), false),
         name.location
      );
   }
}

void Interpreter::visit(const ImportDeclaration& node) {
   loadImport(node);
}

void Interpreter::visit(const BlockStatement& node) {
   executeBlock(node, true);
}

void Interpreter::executeBlock(const BlockStatement& block, bool createScope) {
   if (createScope) {
      environment_.pushScope();
   }

   try {
      for (const auto& statement : block.statements()) {
         if (statement) {
            statement->accept(*this);
         }
      }
   } catch (...) {
      if (createScope) {
         environment_.popScope();
      }
      throw;
   }

   if (createScope) {
      environment_.popScope();
   }
}

void Interpreter::visit(const ExpressionStatement& node) {
   evaluate(node.expression());
}

void Interpreter::visit(const VariableDeclarationStatement& node) {
   RuntimeType type = runtimeTypeFromNode(node.type());
   Value value = node.initializer() == nullptr
      ? defaultValueFor(type)
      : coerceForAssignment(evaluate(*node.initializer()), type, node.location());

   for (const auto& name : node.names()) {
      environment_.defineVariable(
         name.name,
         std::make_shared<ValueObject>(cloneValue(value), node.isMutable()),
         name.location
      );
   }
}

void Interpreter::visit(const ReturnStatement& node) {
   Value value = node.expression() == nullptr ? Value::voidValue() : evaluate(*node.expression());
   throw ReturnSignal{std::move(value)};
}

void Interpreter::visit(const BreakStatement&) {
   throw BreakSignal{};
}

void Interpreter::visit(const ContinueStatement&) {
   throw ContinueSignal{};
}

void Interpreter::visit(const IfStatement& node) {
   if (asBool(evaluate(node.condition()), node.condition().location())) {
      node.thenBranch().accept(*this);
      return;
   }

   if (node.elseBranch() != nullptr) {
      node.elseBranch()->accept(*this);
   }
}

void Interpreter::visit(const WhileStatement& node) {
   while (asBool(evaluate(node.condition()), node.condition().location())) {
      try {
         node.body().accept(*this);
      } catch (const ContinueSignal&) {
         continue;
      } catch (const BreakSignal&) {
         break;
      }
   }
}

void Interpreter::visit(const ForStatement& node) {
   Value iterable = evaluate(node.iterable());
   RuntimeType loopType = runtimeTypeFromNode(node.variableType());

   if (iterable.type().kind() != RuntimeType::Kind::List) {
      reportRuntimeError(RuntimeError(
         "for iterable must be " + RuntimeType::listOf(loopType).toString() + "; continuing with an empty list",
         node.iterable().location()
      ));
      iterable = Value::listValue(loopType, {});
   } else if (iterable.type().elementType() != loopType) {
      reportRuntimeError(RuntimeError(
         "for loop variable type " + loopType.toString() +
         " does not match iterable element type " + iterable.type().elementType().toString() +
         "; continuing with corrected loop type",
         node.variableLocation()
      ));
      loopType = iterable.type().elementType();
   }

   environment_.pushScope();
   try {
      for (const auto& element : std::get<ValueList>(iterable.data())) {
         environment_.pushScope();
         environment_.defineVariable(
            node.variableName(),
            std::make_shared<ValueObject>(cloneValue(element), false),
            node.variableLocation()
         );

         try {
            executeBlock(node.body(), false);
         } catch (const ContinueSignal&) {
            environment_.popScope();
            continue;
         } catch (const BreakSignal&) {
            environment_.popScope();
            break;
         }

         environment_.popScope();
      }
   } catch (...) {
      environment_.popScope();
      throw;
   }
   environment_.popScope();
}

void Interpreter::visit(const AssignmentStatement& node) {
   ValueRef target = resolveAssignable(node.target());
   if (!target->isMutable()) {
      reportRuntimeError(RuntimeError(
         "cannot assign to immutable variable; assignment skipped",
         node.target().location()
      ));
      return;
   }

   target->assign(coerceForAssignment(evaluate(node.value()), target->type(), node.value().location()));
}

void Interpreter::visit(const IntLiteralExpression& node) { lastValue_ = Value::intValue(node.value()); }
void Interpreter::visit(const FloatLiteralExpression& node) { lastValue_ = Value::floatValue(node.value()); }
void Interpreter::visit(const BoolLiteralExpression& node) { lastValue_ = Value::boolValue(node.value()); }
void Interpreter::visit(const CharLiteralExpression& node) { lastValue_ = Value::charValue(node.value()); }
void Interpreter::visit(const StringLiteralExpression& node) { lastValue_ = Value::stringValue(node.value()); }

void Interpreter::visit(const ListLiteralExpression& node) {
   ValueList values;
   std::optional<RuntimeType> elementType;

   for (const auto& element : node.elements()) {
      Value value = evaluate(*element);
      if (!elementType) {
         elementType = value.type();
      } else if (value.type() != *elementType) {
         reportRuntimeError(RuntimeError(
            "mixed type list literal is not supported; element skipped",
            element->location()
         ));
         continue;
      }
      values.push_back(std::move(value));
   }

   lastValue_ = Value::listValue(elementType.value_or(RuntimeType::voidType()), std::move(values));
}

void Interpreter::visit(const IdentifierExpression& node) {
   if (auto value = environment_.tryFindVariable(node.name())) {
      lastValue_ = cloneValue(value->value());
      return;
   }

   if (activeModule_ != nullptr) {
      auto found = activeModule_->constants.find(node.name());
      if (found != activeModule_->constants.end()) {
         lastValue_ = cloneValue(found->second->value());
         return;
      }
   }

   throw RuntimeError("variable not found: " + node.name(), node.location());
}

void Interpreter::visit(const ThisExpression& node) {
   if (!currentThis_) {
      throw RuntimeError("'this' is only available inside list operator expressions", node.location());
   }

   lastValue_ = cloneValue(*currentThis_);
}

void Interpreter::visit(const AddExpression& node) {
   Value left = evaluate(node.left());
   Value right = evaluate(node.right());

   if (left.type().kind() == RuntimeType::Kind::String && right.type().kind() == RuntimeType::Kind::String) {
      lastValue_ = Value::stringValue(std::get<std::string>(left.data()) + std::get<std::string>(right.data()));
      return;
   }

   if (left.type().kind() == RuntimeType::Kind::List && right.type().kind() == RuntimeType::Kind::List && left.type() == right.type()) {
      ValueList merged = std::get<ValueList>(left.data());
      const auto& rhs = std::get<ValueList>(right.data());
      merged.insert(merged.end(), rhs.begin(), rhs.end());
      lastValue_ = Value::listValue(left.type().elementType(), std::move(merged));
      return;
   }

   lastValue_ = evaluateBinaryNumeric(node, '+');
}

void Interpreter::visit(const SubtractExpression& node) {
   Value left = evaluate(node.left());
   Value right = evaluate(node.right());

   if (left.type().kind() == RuntimeType::Kind::List || right.type().kind() == RuntimeType::Kind::List) {
      requireSameListOperands(left, right, "-", node.location());

      ValueList result = std::get<ValueList>(left.data());
      for (const auto& elementToRemove : std::get<ValueList>(right.data())) {
         auto found = std::find_if(result.begin(), result.end(), [&](const Value& candidate) {
            return valuesEqual(candidate, elementToRemove);
         });

         if (found != result.end()) {
            result.erase(found);
         }
      }

      lastValue_ = Value::listValue(left.type().elementType(), std::move(result));
      return;
   }

   lastValue_ = evaluateBinaryNumeric(node, '-');
}

void Interpreter::visit(const MultiplyExpression& node) {
   Value left = evaluate(node.left());
   Value right = evaluate(node.right());

   if (left.type().kind() == RuntimeType::Kind::List || right.type().kind() == RuntimeType::Kind::List) {
      requireSameListOperands(left, right, "*", node.location());

      ValueList result;
      ValueList remainingRight = std::get<ValueList>(right.data());
      for (const auto& leftElement : std::get<ValueList>(left.data())) {
         auto found = std::find_if(remainingRight.begin(), remainingRight.end(), [&](const Value& rightElement) {
            return valuesEqual(leftElement, rightElement);
         });

         if (found != remainingRight.end()) {
            result.push_back(cloneValue(leftElement));
            remainingRight.erase(found);
         }
      }

      lastValue_ = Value::listValue(left.type().elementType(), std::move(result));
      return;
   }

   lastValue_ = evaluateBinaryNumeric(node, '*');
}
void Interpreter::visit(const DivideExpression& node) { lastValue_ = evaluateBinaryNumeric(node, '/'); }
void Interpreter::visit(const PowerExpression& node) { lastValue_ = evaluateBinaryNumeric(node, '^'); }

void Interpreter::visit(const LogicalOrExpression& node) {
   Value left = evaluate(node.left());
   if (asBool(left, node.left().location())) {
      lastValue_ = Value::boolValue(true);
      return;
   }
   lastValue_ = Value::boolValue(asBool(evaluate(node.right()), node.right().location()));
}

void Interpreter::visit(const LogicalAndExpression& node) {
   Value left = evaluate(node.left());
   if (!asBool(left, node.left().location())) {
      lastValue_ = Value::boolValue(false);
      return;
   }
   lastValue_ = Value::boolValue(asBool(evaluate(node.right()), node.right().location()));
}

void Interpreter::visit(const EqualExpression& node) { lastValue_ = Value::boolValue(valuesEqual(evaluate(node.left()), evaluate(node.right()))); }
void Interpreter::visit(const NotEqualExpression& node) { lastValue_ = Value::boolValue(!valuesEqual(evaluate(node.left()), evaluate(node.right()))); }

void Interpreter::visit(const ContainsExpression& node) {
   Value left = evaluate(node.left());
   Value right = evaluate(node.right());

   requireSameListOperands(left, right, "contains", node.location());

   const auto& leftElements = std::get<ValueList>(left.data());
   const auto& rightElements = std::get<ValueList>(right.data());
   lastValue_ = Value::boolValue(std::all_of(rightElements.begin(), rightElements.end(), [&](const Value& required) {
      return std::any_of(leftElements.begin(), leftElements.end(), [&](const Value& candidate) {
         return valuesEqual(candidate, required);
      });
   }));
}

void Interpreter::visit(const LessExpression& node) { lastValue_ = evaluateComparison(node, "<"); }
void Interpreter::visit(const LessEqualExpression& node) { lastValue_ = evaluateComparison(node, "<="); }
void Interpreter::visit(const GreaterExpression& node) { lastValue_ = evaluateComparison(node, ">"); }
void Interpreter::visit(const GreaterEqualExpression& node) { lastValue_ = evaluateComparison(node, ">="); }

void Interpreter::visit(const MapExpression& node) {
   Value list = evaluate(node.left());
   if (list.type().kind() != RuntimeType::Kind::List) {
      throw RuntimeError("map operator requires list on the left side", node.left().location());
   }

   ValueList mapped;
   std::optional<RuntimeType> elementType;
   for (const auto& element : std::get<ValueList>(list.data())) {
      Value value = evaluateWithThis(node.right(), element);
      if (!elementType) {
         elementType = value.type();
      } else if (value.type() != *elementType) {
         throw RuntimeError("map expression must produce homogeneous list", node.right().location());
      }
      mapped.push_back(std::move(value));
   }

   lastValue_ = Value::listValue(elementType.value_or(RuntimeType::voidType()), std::move(mapped));
}

void Interpreter::visit(const FilterExpression& node) {
   Value list = evaluate(node.left());
   if (list.type().kind() != RuntimeType::Kind::List) {
      throw RuntimeError("filter operator requires list on the left side", node.left().location());
   }

   ValueList filtered;
   for (const auto& element : std::get<ValueList>(list.data())) {
      if (asBool(evaluateWithThis(node.right(), element), node.right().location())) {
         filtered.push_back(cloneValue(element));
      }
   }

   lastValue_ = Value::listValue(list.type().elementType(), std::move(filtered));
}

void Interpreter::visit(const GroupExpression& node) {
   Value list = evaluate(node.left());
   if (list.type().kind() != RuntimeType::Kind::List) {
      throw RuntimeError("group operator requires list on the left side", node.left().location());
   }

   std::vector<Value> keys;
   std::vector<ValueList> groups;
   std::optional<RuntimeType> keyType;

   for (const auto& element : std::get<ValueList>(list.data())) {
      Value key = evaluateWithThis(node.right(), element);
      if (!keyType) {
         keyType = key.type();
      } else if (key.type() != *keyType) {
         throw RuntimeError("group expression must produce homogeneous keys", node.right().location());
      }

      auto found = std::find_if(keys.begin(), keys.end(), [&](const Value& existingKey) {
         return valuesEqual(existingKey, key);
      });

      if (found == keys.end()) {
         keys.push_back(std::move(key));
         ValueList newGroup;
         newGroup.push_back(cloneValue(element));
         groups.push_back(std::move(newGroup));
      } else {
         std::size_t index = static_cast<std::size_t>(std::distance(keys.begin(), found));
         groups[index].push_back(cloneValue(element));
      }
   }

   ValueList grouped;
   for (auto& group : groups) {
      grouped.push_back(Value::listValue(list.type().elementType(), std::move(group)));
   }

   lastValue_ = Value::listValue(RuntimeType::listOf(list.type().elementType()), std::move(grouped));
}

void Interpreter::visit(const NegateExpression& node) {
   Value value = evaluate(node.operand());
   if (!isNumeric(value.type())) {
      throw RuntimeError("unary '-' requires numeric operand", node.location());
   }

   if (value.type().kind() == RuntimeType::Kind::Float) {
      lastValue_ = Value::floatValue(-std::get<double>(value.data()));
   } else {
      lastValue_ = makeNumericResult(value.type(), -toDouble(value), -toSignedInteger(value));
   }
}

void Interpreter::visit(const NotExpression& node) {
   Value value = evaluate(node.operand());
   lastValue_ = Value::boolValue(!asBool(value, node.location()));
}

void Interpreter::visit(const CountExpression& node) {
   Value value = evaluate(node.operand());
   if (value.type().kind() == RuntimeType::Kind::String) {
      lastValue_ = Value::intValue(static_cast<int64_t>(std::get<std::string>(value.data()).size()));
      return;
   }
   if (value.type().kind() == RuntimeType::Kind::List) {
      lastValue_ = Value::intValue(static_cast<int64_t>(std::get<ValueList>(value.data()).size()));
      return;
   }
   throw RuntimeError("count requires string or list", node.location());
}

void Interpreter::visit(const ReverseExpression& node) {
   Value value = evaluate(node.operand());
   if (value.type().kind() == RuntimeType::Kind::String) {
      auto text = std::get<std::string>(value.data());
      std::reverse(text.begin(), text.end());
      lastValue_ = Value::stringValue(std::move(text));
      return;
   }
   if (value.type().kind() == RuntimeType::Kind::List) {
      auto elements = std::get<ValueList>(value.data());
      std::reverse(elements.begin(), elements.end());
      lastValue_ = Value::listValue(value.type().elementType(), std::move(elements));
      return;
   }
   throw RuntimeError("reverse requires string or list", node.location());
}

void Interpreter::visit(const FlattenExpression& node) {
   Value value = evaluate(node.operand());
   if (value.type().kind() != RuntimeType::Kind::List ||
       value.type().elementType().kind() != RuntimeType::Kind::List) {
      throw RuntimeError("flatten requires nested list", node.location());
   }

   RuntimeType elementType = value.type().elementType().elementType();
   ValueList flattened;
   for (const auto& nested : std::get<ValueList>(value.data())) {
      const auto& nestedElements = std::get<ValueList>(nested.data());
      flattened.insert(flattened.end(), nestedElements.begin(), nestedElements.end());
   }

   lastValue_ = Value::listValue(elementType, std::move(flattened));
}

void Interpreter::visit(const CastExpression& node) {
   try {
      lastValue_ = castValue(evaluate(node.expression()), runtimeTypeFromNode(node.targetType()));
   } catch (const RuntimeValueOutOfRange& error) {
      reportRuntimeError(RuntimeError(error.what(), node.location()));
      lastValue_ = cloneValue(error.wrappedValue());
   } catch (const RuntimeValueInvalidStringCast& error) {
      throw RuntimeError(error.what(), node.location());
   }
}

void Interpreter::visit(const MemberAccessExpression& node) {
   const auto* moduleName = dynamic_cast<const IdentifierExpression*>(&node.object());
   if (moduleName == nullptr) {
      throw RuntimeError("member access must have form module.member", node.location());
   }

   ImportedModule& module = findImportedModule(moduleName->name(), node.object().location());
   lastValue_ = cloneValue(findImportedConstant(module, node.memberName(), node.location())->value());
}

void Interpreter::visit(const CallExpression& node) {
   std::vector<Value> args;
   for (const auto& argument : node.arguments()) {
      args.push_back(evaluate(*argument));
   }

   if (const auto* name = dynamic_cast<const IdentifierExpression*>(&node.callee())) {
      if (Callable* callable = environment_.tryFindFunction(name->name())) {
         if (callable->arity() != args.size()) {
            throw RuntimeError("wrong number of arguments for function: " + name->name(), node.location());
         }

         lastValue_ = callable->call(*this, args, node.location());
         return;
      }

      if (activeModule_ != nullptr) {
         auto found = activeModule_->functions.find(name->name());
         if (found != activeModule_->functions.end()) {
            const FunctionDeclaration& declaration = *found->second;
            if (declaration.parameters().size() != args.size()) {
               throw RuntimeError("wrong number of arguments for function: " + name->name(), node.location());
            }

            lastValue_ = callImportedFunction(*activeModule_, declaration, args, node.location());
            return;
         }
      }

      throw RuntimeError("function not found: " + name->name(), node.location());
   }

   if (const auto* member = dynamic_cast<const MemberAccessExpression*>(&node.callee())) {
      const auto* moduleName = dynamic_cast<const IdentifierExpression*>(&member->object());
      if (moduleName == nullptr) {
         throw RuntimeError("member function call must have form module.member()", node.callee().location());
      }

      ImportedModule& module = findImportedModule(moduleName->name(), member->object().location());
      const FunctionDeclaration& declaration = findImportedFunction(module, member->memberName(), node.callee().location());

      if (declaration.parameters().size() != args.size()) {
         throw RuntimeError("wrong number of arguments for function: " + moduleName->name() + "." + member->memberName(), node.location());
      }

      lastValue_ = callImportedFunction(module, declaration, args, node.location());
      return;
   }

   throw RuntimeError("only direct calls and imported member calls are supported", node.callee().location());
}

void Interpreter::visit(const IndexExpression& node) {
   Value object = evaluate(node.object());
   Value index = evaluate(node.index());
   if (index.type().kind() != RuntimeType::Kind::Int) {
      throw RuntimeError("index must be int", node.index().location());
   }

   int64_t rawIndex = std::get<int64_t>(index.data());
   if (rawIndex < 0) {
      throw RuntimeError("index cannot be negative", node.index().location());
   }

   std::size_t i = static_cast<std::size_t>(rawIndex);
   if (object.type().kind() == RuntimeType::Kind::String) {
      const auto& text = std::get<std::string>(object.data());
      if (i >= text.size()) {
         throw RuntimeError("string index out of range", node.location());
      }
      lastValue_ = Value::charValue(text[i]);
      return;
   }

   if (object.type().kind() == RuntimeType::Kind::List) {
      const auto& elements = std::get<ValueList>(object.data());
      if (i >= elements.size()) {
         throw RuntimeError("list index out of range", node.location());
      }
      lastValue_ = cloneValue(elements[i]);
      return;
   }

   throw RuntimeError("indexing requires string or list", node.object().location());
}

void Interpreter::visit(const SliceExpression& node) {
   Value object = evaluate(node.object());
   if (object.type().kind() != RuntimeType::Kind::List && object.type().kind() != RuntimeType::Kind::String) {
      throw RuntimeError("slicing requires string or list", node.object().location());
   }

   auto readIndex = [&](const Expression* expression, std::size_t fallback) {
      if (expression == nullptr) {
         return fallback;
      }
      Value value = evaluate(*expression);
      if (value.type().kind() != RuntimeType::Kind::Int || std::get<int64_t>(value.data()) < 0) {
         throw RuntimeError("slice index must be non-negative int", expression->location());
      }
      return static_cast<std::size_t>(std::get<int64_t>(value.data()));
   };

   if (object.type().kind() == RuntimeType::Kind::String) {
      const auto& text = std::get<std::string>(object.data());
      std::size_t start = std::min(readIndex(node.start(), 0), text.size());
      std::size_t end = std::min(readIndex(node.end(), text.size()), text.size());
      if (end < start) {
         end = start;
      }
      lastValue_ = Value::stringValue(text.substr(start, end - start));
      return;
   }

   const auto& elements = std::get<ValueList>(object.data());
   std::size_t start = std::min(readIndex(node.start(), 0), elements.size());
   std::size_t end = std::min(readIndex(node.end(), elements.size()), elements.size());
   if (end < start) {
      end = start;
   }
   ValueList sliced(elements.begin() + static_cast<std::ptrdiff_t>(start), elements.begin() + static_cast<std::ptrdiff_t>(end));
   lastValue_ = Value::listValue(object.type().elementType(), std::move(sliced));
}

Value Interpreter::evaluate(const Expression& expression) {
   expression.accept(*this);
   if (!lastValue_) {
      throw RuntimeError("expression did not produce a value", expression.location());
   }
   return *lastValue_;
}

Value Interpreter::evaluateWithThis(const Expression& expression, const Value& thisValue) {
   auto previousThis = currentThis_;
   currentThis_ = cloneValue(thisValue);

   try {
      Value value = evaluate(expression);
      currentThis_ = std::move(previousThis);
      return value;
   } catch (...) {
      currentThis_ = std::move(previousThis);
      throw;
   }
}

ValueRef Interpreter::resolveAssignable(const Expression& expression) {
   if (const auto* identifier = dynamic_cast<const IdentifierExpression*>(&expression)) {
      return environment_.findVariable(identifier->name(), identifier->location());
   }

   throw RuntimeError("assignment target is not supported yet", expression.location());
}

void Interpreter::loadImports(const Program& program) {
   for (const auto& declaration : program.imports()) {
      declaration->accept(*this);
   }
}

Interpreter::ImportedModule& Interpreter::loadImport(const ImportDeclaration& declaration) {
   std::filesystem::path resolvedPath = resolveImportPath(declaration.path());
   const std::string moduleName = moduleNameForPath(resolvedPath);
   const std::string pathKey = resolvedPath.string();

   if (moduleName.empty()) {
      throw RuntimeError("import path does not produce a module name: " + declaration.path(), declaration.location());
   }

   if (loadingImportPaths_.find(pathKey) != loadingImportPaths_.end()) {
      throw RuntimeError("cyclic import detected: " + resolvedPath.string(), declaration.location());
   }

   auto existing = importedModules_.find(moduleName);
   if (existing != importedModules_.end()) {
      if (existing->second.path != resolvedPath) {
         throw RuntimeError("import module name already used: " + moduleName, declaration.location());
      }

      exportImportedNames(existing->second, declaration);
      return existing->second;
   }

   loadingImportPaths_.insert(pathKey);

   try {
      std::size_t errorCountBefore = errorHandler_ == nullptr ? 0 : errorHandler_->errorCount();

      FileSource source(resolvedPath.string());
      Lexer lexer(source);
      Parser parser(lexer, errorHandler_);
      ProgramPtr importedProgram = parser.parseProgram();

      if (importedProgram == nullptr ||
          (errorHandler_ != nullptr && errorHandler_->errorCount() > errorCountBefore)) {
         throw RuntimeError("failed to parse imported file: " + resolvedPath.string(), declaration.location());
      }

      ImportedModule module;
      module.path = resolvedPath;
      module.program = std::move(importedProgram);

      auto inserted = importedModules_.emplace(moduleName, std::move(module));
      ImportedModule& importedModule = inserted.first->second;

      buildImportedModule(importedModule);
      exportImportedNames(importedModule, declaration);

      loadingImportPaths_.erase(pathKey);
      return importedModule;
   } catch (const RuntimeError&) {
      loadingImportPaths_.erase(pathKey);
      throw;
   } catch (const std::exception& error) {
      loadingImportPaths_.erase(pathKey);
      throw RuntimeError("cannot import file '" + resolvedPath.string() + "': " + error.what(), declaration.location());
   }
}

void Interpreter::buildImportedModule(ImportedModule& module) {
   loadImports(*module.program);

   for (const auto& function : module.program->functionDeclarations()) {
      module.functions[function->name()] = function.get();
   }

   ImportedModule* previousModule = activeModule_;
   activeModule_ = &module;

   try {
      for (const auto& declaration : module.program->globalConstantDeclarations()) {
         evaluateImportedGlobalConstant(*declaration, module);
      }
   } catch (...) {
      activeModule_ = previousModule;
      throw;
   }

   activeModule_ = previousModule;
}

void Interpreter::exportImportedNames(ImportedModule& module, const ImportDeclaration& declaration) {
   if (declaration.importAll()) {
      module.exportsAll = true;
      return;
   }

   for (const auto& name : declaration.importedNames()) {
      if (module.functions.find(name) == module.functions.end() &&
          module.constants.find(name) == module.constants.end()) {
         throw RuntimeError("imported name not found: " + name, declaration.location());
      }

      module.exportedNames.insert(name);
   }
}

std::filesystem::path Interpreter::resolveImportPath(const std::string& importPath) const {
   std::filesystem::path path(importPath);
   if (path.is_relative()) {
      path = importRoot_ / path;
   }

   path = std::filesystem::absolute(path).lexically_normal();

   std::error_code error;
   std::filesystem::path canonical = std::filesystem::weakly_canonical(path, error);
   if (!error) {
      return canonical;
   }

   return path;
}

std::string Interpreter::moduleNameForPath(const std::filesystem::path& path) const {
   return path.stem().string();
}

Interpreter::ImportedModule& Interpreter::findImportedModule(const std::string& name, const SourceLocation& location) {
   auto found = importedModules_.find(name);
   if (found == importedModules_.end()) {
      throw RuntimeError("imported module not found: " + name, location);
   }

   return found->second;
}

const FunctionDeclaration& Interpreter::findImportedFunction(
   ImportedModule& module,
   const std::string& name,
   const SourceLocation& location
) const {
   if (!isExported(module, name)) {
      throw RuntimeError("imported member is not exported: " + module.path.stem().string() + "." + name, location);
   }

   auto found = module.functions.find(name);
   if (found == module.functions.end()) {
      if (module.constants.find(name) != module.constants.end()) {
         throw RuntimeError("imported member is not a function: " + module.path.stem().string() + "." + name, location);
      }

      throw RuntimeError("imported function not found: " + module.path.stem().string() + "." + name, location);
   }

   return *found->second;
}

ValueRef Interpreter::findImportedConstant(
   ImportedModule& module,
   const std::string& name,
   const SourceLocation& location
) const {
   if (!isExported(module, name)) {
      throw RuntimeError("imported member is not exported: " + module.path.stem().string() + "." + name, location);
   }

   auto found = module.constants.find(name);
   if (found == module.constants.end()) {
      if (module.functions.find(name) != module.functions.end()) {
         throw RuntimeError("imported member is a function and must be called: " + module.path.stem().string() + "." + name, location);
      }

      throw RuntimeError("imported constant not found: " + module.path.stem().string() + "." + name, location);
   }

   return found->second;
}

bool Interpreter::isExported(const ImportedModule& module, const std::string& name) const {
   return module.exportsAll || module.exportedNames.find(name) != module.exportedNames.end();
}

void Interpreter::evaluateImportedGlobalConstant(const GlobalConstantDeclaration& declaration, ImportedModule& module) {
   RuntimeType type = runtimeTypeFromNode(declaration.type());
   Value value = declaration.initializer() == nullptr
      ? defaultValueFor(type)
      : coerceForAssignment(evaluate(*declaration.initializer()), type, declaration.location());

   for (const auto& name : declaration.names()) {
      if (!module.constants.emplace(name.name, std::make_shared<ValueObject>(cloneValue(value), false)).second) {
         throw RuntimeError("global constant already defined in import: " + name.name, name.location);
      }
   }
}

Value Interpreter::callImportedFunction(
   ImportedModule& module,
   const FunctionDeclaration& declaration,
   const std::vector<Value>& args,
   const SourceLocation&
) {
   ImportedModule* previousModule = activeModule_;
   activeModule_ = &module;

   try {
      Value result = executeUserFunction(declaration, args);
      activeModule_ = previousModule;
      return result;
   } catch (...) {
      activeModule_ = previousModule;
      throw;
   }
}

Value Interpreter::executeMainWithoutParameters(const FunctionDeclaration& declaration) {
   environment_.pushCallContext();

   try {
      declaration.body().accept(*this);

      RuntimeType returnType = runtimeTypeFromNode(declaration.returnType());
      environment_.popCallContext();
      return defaultValueFor(returnType);
   } catch (const ReturnSignal& signal) {
      RuntimeType returnType = runtimeTypeFromNode(declaration.returnType());
      Value value = coerceForAssignment(signal.value, returnType, declaration.location());
      environment_.popCallContext();
      return value;
   } catch (...) {
      environment_.popCallContext();
      throw;
   }
}

Value Interpreter::evaluateBinaryNumeric(const BinaryExpression& node, char operation) {
   Value left = evaluate(node.left());
   Value right = evaluate(node.right());

   if (left.type() != right.type() || !isNumeric(left.type())) {
      throw RuntimeError("numeric operator requires operands of the same numeric type", node.location());
   }

   if (left.type().kind() == RuntimeType::Kind::Float) {
      double l = toDouble(left);
      double r = toDouble(right);
      switch (operation) {
         case '+': return Value::floatValue(l + r);
         case '-': return Value::floatValue(l - r);
         case '*': return Value::floatValue(l * r);
         case '/':
            if (r == 0.0) throw RuntimeError("division by zero", node.location());
            return Value::floatValue(l / r);
         case '^': return Value::floatValue(std::pow(l, r));
      }
   }

   int64_t l = toSignedInteger(left);
   int64_t r = toSignedInteger(right);
   if (operation == '/' && r == 0) {
      throw RuntimeError("division by zero", node.location());
   }

   int64_t result = 0;
   switch (operation) {
      case '+': result = l + r; break;
      case '-': result = l - r; break;
      case '*': result = l * r; break;
      case '/': result = l / r; break;
      case '^': result = static_cast<int64_t>(std::pow(static_cast<double>(l), static_cast<double>(r))); break;
   }

   return makeNumericResult(left.type(), static_cast<double>(result), result);
}

Value Interpreter::evaluateComparison(const BinaryExpression& node, const std::string& operation) {
   Value left = evaluate(node.left());
   Value right = evaluate(node.right());
   int comparison = 0;

   try {
      comparison = compareValues(left, right);
   } catch (const std::runtime_error& error) {
      throw RuntimeError(error.what(), node.location());
   }

   if (operation == "<") return Value::boolValue(comparison < 0);
   if (operation == "<=") return Value::boolValue(comparison <= 0);
   if (operation == ">") return Value::boolValue(comparison > 0);
   return Value::boolValue(comparison >= 0);
}

Value Interpreter::coerceForParameter(
   Value value,
   const RuntimeType& targetType,
   const std::string& parameterName,
   const SourceLocation& location
) {
   if (value.type() == targetType) {
      return value;
   }

   if (value.type().kind() == RuntimeType::Kind::List &&
       targetType.kind() == RuntimeType::Kind::List &&
       std::get<ValueList>(value.data()).empty()) {
      return Value::listValue(targetType.elementType(), {});
   }

   throw RuntimeError(
      "cannot pass " + value.type().toString() +
      " to parameter '" + parameterName +
      "' of type " + targetType.toString(),
      location
   );
}

Value Interpreter::coerceForAssignment(Value value, const RuntimeType& targetType, const SourceLocation& location) {
   if (targetType.kind() == RuntimeType::Kind::Void) {
      if (value.type().kind() == RuntimeType::Kind::Void) {
         return value;
      }
      throw RuntimeError("cannot assign value to void", location);
   }

   if (value.type() == targetType) {
      return value;
   }

   if (value.type().kind() == RuntimeType::Kind::List &&
       targetType.kind() == RuntimeType::Kind::List &&
       std::get<ValueList>(value.data()).empty()) {
      return Value::listValue(targetType.elementType(), {});
   }

   if (isNumeric(value.type()) && isNumeric(targetType)) {
      try {
         return castValue(value, targetType);
      } catch (const RuntimeValueOutOfRange& error) {
         reportRuntimeError(RuntimeError(error.what(), location));
         return cloneValue(error.wrappedValue());
      } catch (const RuntimeValueInvalidStringCast& error) {
         throw RuntimeError(error.what(), location);
      } catch (const std::runtime_error&) {
      }
   }

   throw RuntimeError("cannot assign " + value.type().toString() + " to " + targetType.toString(), location);
}

bool Interpreter::mainSignatureIsValid(const FunctionDeclaration& declaration) const {
   const auto& parameters = declaration.parameters();
   if (parameters.size() != 1) {
      return parameters.empty();
   }

   RuntimeType parameterType = runtimeTypeFromNode(*parameters[0].type);
   return parameterType == RuntimeType::listOf(RuntimeType::stringType());
}

bool Interpreter::asBool(const Value& value, const SourceLocation& location) const {
   if (value.type().kind() != RuntimeType::Kind::Bool) {
      throw RuntimeError("condition must be bool", location);
   }

   return std::get<bool>(value.data());
}

void Interpreter::reportRuntimeError(const RuntimeError& error) {
   errorHandler_->report(ErrorType::Runtime, error.what(), error.location());
   errorHandler_->printLastWarning(*output_);
}
