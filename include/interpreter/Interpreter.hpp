#pragma once

/// @file Interpreter.hpp
/// Visitor-based execution engine for parsed DJM programs.

#include <filesystem>
#include <iosfwd>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "diagnostics/ErrorHandler.hpp"
#include "interpreter/Environment.hpp"
#include "syntax/Declarations.hpp"
#include "syntax/Visitor.hpp"

/// Executes a parsed DJM Program by visiting declarations, statements and expressions.
///
/// The interpreter owns runtime scopes, imported modules and recoverable diagnostic
/// policy. Expression visits publish their result through lastValue_.
class Interpreter final : public Visitor {
public:
   /// Creates an interpreter; null handlers and streams receive safe defaults.
   explicit Interpreter(
      ErrorHandler* errorHandler = nullptr,
      std::ostream* output = nullptr,
      std::string mainFilePath = ""
   );

   /// Executes a complete program and returns the integer process exit code.
   int interpret(const Program& program, const std::vector<std::string>& args = {});

   /// Invokes a user function in a fresh call context.
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

   struct ImportedModule {
      std::filesystem::path path;
      ProgramPtr program;
      std::unordered_map<std::string, const FunctionDeclaration*> functions;
      std::unordered_map<std::string, ValueRef> constants;
      std::unordered_set<std::string> exportedNames;
      bool exportsAll = false;
   };

   Environment environment_;
   NullErrorHandler nullErrorHandler_;
   ErrorHandler* errorHandler_;
   std::ostream* output_;
   std::optional<Value> lastValue_;
   std::optional<Value> currentThis_;
   std::vector<std::string> programArgs_;
   std::filesystem::path importRoot_;
   std::unordered_map<std::string, ImportedModule> importedModules_;
   std::unordered_set<std::string> loadingImportPaths_;
   ImportedModule* activeModule_ = nullptr;

   Value evaluate(const Expression& expression);
   Value evaluateWithThis(const Expression& expression, const Value& thisValue);
   ValueRef resolveAssignable(const Expression& expression);
   void loadImports(const Program& program);
   ImportedModule& loadImport(const ImportDeclaration& declaration);
   void buildImportedModule(ImportedModule& module);
   void exportImportedNames(ImportedModule& module, const ImportDeclaration& declaration);
   std::filesystem::path resolveImportPath(const std::string& importPath) const;
   std::string moduleNameForPath(const std::filesystem::path& path) const;
   ImportedModule& findImportedModule(const std::string& name, const SourceLocation& location);
   const FunctionDeclaration& findImportedFunction(
      ImportedModule& module,
      const std::string& name,
      const SourceLocation& location
   ) const;
   ValueRef findImportedConstant(
      ImportedModule& module,
      const std::string& name,
      const SourceLocation& location
   ) const;
   bool isExported(const ImportedModule& module, const std::string& name) const;
   void evaluateImportedGlobalConstant(const GlobalConstantDeclaration& declaration, ImportedModule& module);
   Value callImportedFunction(
      ImportedModule& module,
      const FunctionDeclaration& declaration,
      const std::vector<Value>& args,
      const SourceLocation& location
   );
   Value executeMainWithoutParameters(const FunctionDeclaration& declaration);
   Value evaluateBinaryNumeric(const BinaryExpression& node, char operation);
   Value evaluateComparison(const BinaryExpression& node, const std::string& operation);
   Value coerceForParameter(
      Value value,
      const RuntimeType& targetType,
      const std::string& parameterName,
      const SourceLocation& location
   );
   Value coerceForAssignment(Value value, const RuntimeType& targetType, const SourceLocation& location);
   bool mainSignatureIsValid(const FunctionDeclaration& declaration) const;
   bool asBool(const Value& value, const SourceLocation& location) const;
   void executeBlock(const BlockStatement& block, bool createScope);
   void reportRuntimeError(const RuntimeError& error);
};
