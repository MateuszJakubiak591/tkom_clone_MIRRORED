#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "syntax/Node.hpp"
#include "syntax/Type.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Expressions.hpp"

class Declaration : public Node {
public:
   explicit Declaration(SourceLocation location)
      : Node(std::move(location)) {}

   ~Declaration() override = default;
};

using DeclPtr = std::unique_ptr<Declaration>;

struct ParameterNode {
   std::string name;
   SourceLocation location;
   std::unique_ptr<TypeNode> type;
};

class FunctionDeclaration : public Declaration {
public:
   FunctionDeclaration(
      SourceLocation location,
      std::string name,
      std::vector<ParameterNode> parameters,
      std::unique_ptr<TypeNode> returnType,
      std::unique_ptr<BlockStatement> body
   )
      : Declaration(std::move(location)),
        name_(std::move(name)),
        parameters_(std::move(parameters)),
        returnType_(std::move(returnType)),
        body_(std::move(body)) {}

   const std::string& name() const {
      return name_;
   }

   const std::vector<ParameterNode>& parameters() const {
      return parameters_;
   }

   const TypeNode& returnType() const {
      return *returnType_;
   }

   const BlockStatement& body() const {
      return *body_;
   }

private:
   std::string name_;
   std::vector<ParameterNode> parameters_;
   std::unique_ptr<TypeNode> returnType_;
   std::unique_ptr<BlockStatement> body_;
};

using FunctionDeclPtr = std::unique_ptr<FunctionDeclaration>;
// Różni się od VariableDeclarationStatement jedynie brakiem
// możliwości mut (mutowalności), można by wyciągnąć
// wspólną część klasy GlobalConstantDeclaration i VariableDeclarationStatement
// do innej klasy
class GlobalConstantDeclaration  : public Declaration {
public:
   GlobalConstantDeclaration(
      SourceLocation location,
      std::unique_ptr<TypeNode> type,
      std::vector<VariableDeclarator> names,
      ExprPtr initializer
   )
      : Declaration(std::move(location)),
        type_(std::move(type)),
        names_(std::move(names)),
        initializer_(std::move(initializer)) {}

   const TypeNode& type() const {
      return *type_;
   }

   const std::vector<VariableDeclarator>& names() const {
      return names_;
   }

   const Expression* initializer() const {
      return initializer_.get();
   }

private:
   std::unique_ptr<TypeNode> type_;
   std::vector<VariableDeclarator> names_;
   ExprPtr initializer_;
};

using GlobalConstDeclPtr = std::unique_ptr<GlobalConstantDeclaration>;

class ImportDeclaration  : public Node {
public:
   ImportDeclaration(
      SourceLocation location,
      bool importAll,
      std::vector<std::string> importedNames,
      std::string path
   )
      : Node(std::move(location)),
        importAll_(importAll),
        importedNames_(std::move(importedNames)),
        path_(std::move(path)) {}

   bool importAll() const {
      return importAll_;
   }

   const std::vector<std::string>& importedNames() const {
      return importedNames_;
   }

   const std::string& path() const {
      return path_;
   }

private:
   bool importAll_;
   std::vector<std::string> importedNames_;
   std::string path_;
};

using ImportDeclPtr = std::unique_ptr<ImportDeclaration>;

class Program  : public Node {
public:
   Program(
      SourceLocation location,
      std::vector<ImportDeclPtr> imports,
      std::vector<DeclPtr> declarations
   )
      : Node(std::move(location)),
        imports_(std::move(imports)),
        declarations_(std::move(declarations)) {}

   const std::vector<ImportDeclPtr>& imports() const {
      return imports_;
   }

   const std::vector<DeclPtr>& declarations() const {
      return declarations_;
   }

private:
   std::vector<ImportDeclPtr> imports_;
   std::vector<DeclPtr> declarations_;
};

using ProgramPtr = std::unique_ptr<Program>;