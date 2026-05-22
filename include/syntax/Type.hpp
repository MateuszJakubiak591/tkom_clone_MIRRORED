#pragma once

#include <memory>
#include <string>

#include "syntax/Node.hpp"

/*
type = value_type | "void";

value_type = basic_value_type
           | list_type
           | user_type;

basic_value_type = "int"
                 | "uint"
                 | "float"
                 | "bool"
                 | "char"
                 | "string";

list_type = "list" , "<" , value_type , ">" ;
user_type = identifier ;
*/

class TypeNode : public Node {
public:
   using Node::Node;
   ~TypeNode() override = default;
};

class VoidTypeNode  : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit VoidTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class IntTypeNode  : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit IntTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class UintTypeNode  : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit UintTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class FloatTypeNode  : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit FloatTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class BoolTypeNode  : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit BoolTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class CharTypeNode  : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit CharTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class StringTypeNode  : public TypeNode {
public:
   // using TypeNode::TypeNode ???
   explicit StringTypeNode(SourceLocation location)
      : TypeNode(std::move(location)) {}
};

class ListTypeNode  : public TypeNode {
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

class UserTypeNode  : public TypeNode {
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