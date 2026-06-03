#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "interpreter/RuntimeValue.hpp"
#include "source/SourceLocation.hpp"

class Interpreter;
class FunctionDeclaration;

class RuntimeError : public std::runtime_error {
public:
   RuntimeError(std::string message, SourceLocation location);

   const SourceLocation& location() const;

private:
   SourceLocation location_;
};

class Callable {
public:
   virtual ~Callable() = default;
   virtual Value call(Interpreter& interpreter, const std::vector<Value>& args, const SourceLocation& location) = 0;
   virtual const RuntimeType& returnType() const = 0;
   virtual std::size_t arity() const = 0;
};

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

class BuiltinFunction final : public Callable {
public:
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

class Scope {
public:
   bool define(const std::string& name, ValueRef value);
   ValueRef findLocal(const std::string& name) const;

private:
   std::unordered_map<std::string, ValueRef> variables_;
};

struct FunctionCallContext {
   std::vector<Scope> scopes;
};

class Environment {
public:
   void addBuiltins();
   void addFunction(const FunctionDeclaration& function);

   Callable* tryFindFunction(const std::string& name) const;
   Callable& findFunction(const std::string& name, const SourceLocation& location) const;

   void pushCallContext();
   void popCallContext();
   bool hasCallContext() const;

   void pushScope();
   void popScope();

   void defineVariable(const std::string& name, ValueRef value, const SourceLocation& location);
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
