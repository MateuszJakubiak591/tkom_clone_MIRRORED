#pragma once

#include <memory>
#include <string>

#include "syntax/Node.hpp"

class TypeNode : public Node {
public:
   using Node::Node;
   ~TypeNode() override = default;
};

class VoidTypeNode final : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit VoidTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class IntTypeNode final : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit IntTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class UintTypeNode final : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit UintTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class FloatTypeNode final : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit FloatTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class BoolTypeNode final : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit BoolTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class CharTypeNode final : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit CharTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class StringTypeNode final : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit StringTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class ListTypeNode final : public TypeNode {
public:
   ListTypeNode(SourceLocation location, std::unique_ptr<TypeNode> elementType)
      : TypeNode(std::move(location)),
        elementType_(std::move(elementType)) {}

   const TypeNode& elementType() const {
      return *elementType_;
   }

private:
   std::unique_ptr<TypeNode> elementType_;
};

class UserTypeNode final : public TypeNode {
public:
   UserTypeNode(SourceLocation location, std::string name)
      : TypeNode(std::move(location)),
        name_(std::move(name)) {}

   const std::string& name() const {
      return name_;
   }

private:
   std::string name_;
};