#pragma once

/// @file Environment.hpp
/// Runtime symbol tables, lexical scopes and callable function adapters.

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "interpreter/RuntimeValue.hpp"
#include "source/SourceLocation.hpp"

class Interpreter;
class FunctionDeclaration;

/// Interpreter error enriched with the source location that caused it.
class RuntimeError : public std::runtime_error {
public:
   RuntimeError(std::string message, SourceLocation location);

   const SourceLocation& location() const;

private:
   SourceLocation location_;
};

/// Common runtime interface for user-defined and built-in functions.
class Callable {
public:
   virtual ~Callable() = default;
   /// Invokes the function with already evaluated arguments.
   virtual Value call(Interpreter& interpreter, const std::vector<Value>& args, const SourceLocation& location) = 0;
   /// Returns the declared runtime return type.
   virtual const RuntimeType& returnType() const = 0;
   /// Returns the exact number of required positional arguments.
   virtual std::size_t arity() const = 0;
};

/// Callable adapter that delegates execution to a FunctionDeclaration AST node.
class UserFunction final : public Callable {
public:
   explicit UserFunction(const FunctionDeclaration& declaration);

   Value call(Interpreter& interpreter, const std::vector<Value>& args, const SourceLocation& location) override;
   const RuntimeType& returnType() const override;
   std::size_t arity() const override;

private:
   const FunctionDeclaration& declaration_;
   RuntimeType returnType_;
};

/// Callable adapter for native C++ functions exposed to DJM programs.
class BuiltinFunction final : public Callable {
public:
   /// Signature used by a built-in implementation after argument validation.
   using Body = Value (*)(const std::vector<Value>& args, const SourceLocation& location);

   BuiltinFunction(std::size_t arity, RuntimeType returnType, Body body);

   Value call(Interpreter& interpreter, const std::vector<Value>& args, const SourceLocation& location) override;
   const RuntimeType& returnType() const override;
   std::size_t arity() const override;

private:
   std::size_t arity_;
   RuntimeType returnType_;
   Body body_;
};

/// Single lexical scope containing variables declared directly in that scope.
class Scope {
public:
   /// Inserts a variable and returns false when the name already exists locally.
   bool define(const std::string& name, ValueRef value);
   /// Finds only a directly declared variable, without searching parent scopes.
   ValueRef findLocal(const std::string& name) const;

private:
   std::unordered_map<std::string, ValueRef> variables_;
};

/// Stack of lexical scopes belonging to one active function invocation.
struct FunctionCallContext {
   std::vector<Scope> scopes;
};

/// Runtime symbol environment for globals, functions and nested call-local scopes.
class Environment {
public:
   /// Registers native functions available to every interpreted program.
   void addBuiltins();
   /// Registers a user function declaration under its source-level name.
   void addFunction(const FunctionDeclaration& function);

   Callable* tryFindFunction(const std::string& name) const;
   Callable& findFunction(const std::string& name, const SourceLocation& location) const;

   /// Starts a function invocation with its initial local scope.
   void pushCallContext();
   /// Removes the most recent invocation and all of its local scopes.
   void popCallContext();
   bool hasCallContext() const;

   /// Starts a nested lexical scope in the current function call.
   void pushScope();
   /// Removes the innermost lexical scope in the current function call.
   void popScope();

   /// Defines a variable in the current local scope or in globals outside a call.
   void defineVariable(const std::string& name, ValueRef value, const SourceLocation& location);
   /// Searches local scopes from inner to outer and then the global scope.
   ValueRef tryFindVariable(const std::string& name) const;
   ValueRef findVariable(const std::string& name, const SourceLocation& location) const;
   Scope& globalScope();

private:
   Scope globalScope_;
   std::unordered_map<std::string, std::unique_ptr<Callable>> functions_;
   std::vector<FunctionCallContext> callStack_;

   FunctionCallContext& currentCallContext();
   const FunctionCallContext& currentCallContext() const;
   Scope& currentScope();
};
