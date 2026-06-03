#include "interpreter/Environment.hpp"

#include <iostream>
#include <utility>

#include "interpreter/Interpreter.hpp"
#include "syntax/Declarations.hpp"

RuntimeError::RuntimeError(std::string message, SourceLocation location)
   : std::runtime_error(std::move(message)),
     location_(std::move(location)) {}

const SourceLocation& RuntimeError::location() const {
   return location_;
}

UserFunction::UserFunction(const FunctionDeclaration& declaration)
   : declaration_(declaration),
     returnType_(runtimeTypeFromNode(declaration.returnType())) {}

Value UserFunction::call(Interpreter& interpreter, const std::vector<Value>& args, const SourceLocation&) {
   return interpreter.executeUserFunction(declaration_, args);
}

const RuntimeType& UserFunction::returnType() const {
   return returnType_;
}

std::size_t UserFunction::arity() const {
   return declaration_.parameters().size();
}

BuiltinFunction::BuiltinFunction(std::size_t arity, RuntimeType returnType, Body body)
   : arity_(arity),
     returnType_(std::move(returnType)),
     body_(body) {}

Value BuiltinFunction::call(Interpreter&, const std::vector<Value>& args, const SourceLocation& location) {
   return body_(args, location);
}

const RuntimeType& BuiltinFunction::returnType() const {
   return returnType_;
}

std::size_t BuiltinFunction::arity() const {
   return arity_;
}

bool Scope::define(const std::string& name, ValueRef value) {
   return variables_.emplace(name, std::move(value)).second;
}

ValueRef Scope::findLocal(const std::string& name) const {
   auto found = variables_.find(name);
   if (found == variables_.end()) {
      return nullptr;
   }

   return found->second;
}

void Environment::addBuiltins() {
   functions_["print"] = std::make_unique<BuiltinFunction>(
      1,
      RuntimeType::voidType(),
      [](const std::vector<Value>& args, const SourceLocation&) {
         std::cout << args[0].toString();
         return Value::voidValue();
      }
   );

   functions_["println"] = std::make_unique<BuiltinFunction>(
      1,
      RuntimeType::voidType(),
      [](const std::vector<Value>& args, const SourceLocation&) {
         std::cout << args[0].toString() << '\n';
         return Value::voidValue();
      }
   );
}

void Environment::addFunction(const FunctionDeclaration& function) {
   functions_[function.name()] = std::make_unique<UserFunction>(function);
}

Callable* Environment::tryFindFunction(const std::string& name) const {
   auto found = functions_.find(name);
   if (found == functions_.end()) {
      return nullptr;
   }

   return found->second.get();
}

Callable& Environment::findFunction(const std::string& name, const SourceLocation& location) const {
   if (auto* function = tryFindFunction(name)) {
      return *function;
   }

   throw RuntimeError("function not found: " + name, location);
}

void Environment::pushCallContext() {
   FunctionCallContext context;
   context.scopes.emplace_back();
   callStack_.push_back(std::move(context));
}

void Environment::popCallContext() {
   callStack_.pop_back();
}

bool Environment::hasCallContext() const {
   return !callStack_.empty();
}

void Environment::pushScope() {
   currentCallContext().scopes.emplace_back();
}

void Environment::popScope() {
   currentCallContext().scopes.pop_back();
}

void Environment::defineVariable(const std::string& name, ValueRef value, const SourceLocation& location) {
   Scope& target = hasCallContext() ? currentScope() : globalScope_;

   if (!target.define(name, std::move(value))) {
      throw RuntimeError("variable already defined in this scope: " + name, location);
   }
}

ValueRef Environment::tryFindVariable(const std::string& name) const {
   if (hasCallContext()) {
      const auto& scopes = currentCallContext().scopes;
      for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
         if (auto value = it->findLocal(name)) {
            return value;
         }
      }
   }

   if (auto value = globalScope_.findLocal(name)) {
      return value;
   }

   return nullptr;
}

ValueRef Environment::findVariable(const std::string& name, const SourceLocation& location) const {
   if (auto value = tryFindVariable(name)) {
      return value;
   }

   throw RuntimeError("variable not found: " + name, location);
}

Scope& Environment::globalScope() {
   return globalScope_;
}

FunctionCallContext& Environment::currentCallContext() {
   if (callStack_.empty()) {
      throw RuntimeError("no active function call context", SourceLocation{});
   }

   return callStack_.back();
}

const FunctionCallContext& Environment::currentCallContext() const {
   if (callStack_.empty()) {
      throw RuntimeError("no active function call context", SourceLocation{});
   }

   return callStack_.back();
}

Scope& Environment::currentScope() {
   auto& scopes = currentCallContext().scopes;
   return scopes.back();
}
