#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "syntax/Node.hpp"
#include "syntax/Expressions.hpp"
#include "syntax/Type.hpp"

/*
statement = variable_decl_stmt
         | assignment_stmt
         | expression
         | return_stmt
         | if_stmt
         | while_stmt
         | for_stmt
         | block ;
*/

class Statement : public Node {
public:
   using Node::Node;
   ~Statement() override = default;
};

using StmtPtr = std::unique_ptr<Statement>;

/*
block = "{" , { statement } , "}" ;
*/

class BlockStatement final : public Statement {
public:
   BlockStatement(SourceLocation location, std::vector<StmtPtr> statements)
      : Statement(std::move(location)),
        statements_(std::move(statements)) {}

   const std::vector<StmtPtr>& statements() const {
      return statements_;
   }

private:
   std::vector<StmtPtr> statements_;
};

class ExpressionStatement final : public Statement {
public:
   ExpressionStatement(SourceLocation location, ExprPtr expression)
      : Statement(std::move(location)),
        expression_(std::move(expression)) {}

   const Expression& expression() const {
      return *expression_;
   }

private:
   ExprPtr expression_;
};

/*
variable_decl_stmt	= [ "mut" ] , 	value_type , identifier_list , [ "=" , expression ] ;

Język umożliwia np.
mut a, b, c = 5

Stąd oddzielnie VariableDeclarator, przezchowuje lokalizację każdej
z powyższych zmiennych a, b, c (chociaż może to nadmiarowe, i
wystarczyłaby sama linijka)
*/

struct VariableDeclarator {
   std::string name;
   SourceLocation location;
};

class VariableDeclarationStatement final : public Statement {
public:
   VariableDeclarationStatement(
      SourceLocation location,
      bool isMutable,
      std::unique_ptr<TypeNode> type,
      std::vector<VariableDeclarator> names,
      ExprPtr initializer
   )
      : Statement(std::move(location)),
        isMutable_(isMutable),
        type_(std::move(type)),
        names_(std::move(names)),
        initializer_(std::move(initializer)) {}

   bool isMutable() const {
      return isMutable_;
   }

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
   bool isMutable_;
   std::unique_ptr<TypeNode> type_;
   std::vector<VariableDeclarator> names_;
   ExprPtr initializer_;   // jeśli zmienne niezainicjalizowane, to może być
                           // tutaj jako ExprPtr nullptr
};

/*
return_stmt = "return" , [ expression ] ;
*/

class ReturnStatement final : public Statement {
public:
   ReturnStatement(SourceLocation location, ExprPtr expression)
      : Statement(std::move(location)),
        expression_(std::move(expression)) {}

   const Expression* expression() const {
      return expression_.get();
   }

private:
   ExprPtr expression_;
};

/*
///////////////////////   STARA GRAMATYKA //////////////////////////////////////
statement       = variable_decl_stmt
               | assignment_stmt
               | expression
               | return_stmt
               | if_stmt
               | while_stmt
               | for_stmt
               | block ;
assignment_postfix 	= "." , identifier	| index_suffix ;
assignment_target 	= identifier , { assignment_postfix } ; 
assignment_stmt = assignment_target , "=" , expression ;

expression = logical_or_expr ;

primary_expr    = literal
               | identifier
               | "(" , expression , ")"
               | "this" ;

Mamy problem, bo assignment_target zaczyna się od identifier, a expression
poprzez primary_expr także może zaczynać się od identifier.

W związku z tym proponowana jest poniższa zmiana
////////////////////////////////////////////////////////////////////////////////

///////////////////////   NOWA GRAMATYKA //////////////////////////////////////
statement       = variable_decl_stmt
               | expression_or_assignment_stmt
               | return_stmt
               | if_stmt
               | while_stmt
               | for_stmt
               | block ;

expression_or_assignment_stmt
               = expression , [ "=" , expression ] ;
            
Teraz parser parsuje expression, i jeśli napotka token '='
to przypisuje sparsowane expression. Można potem sprawdzić,
jakiego typu jest expression po lewej stronie,
i zgłosić błąd jeśli nie jest to:
IdentifierExpression, MemeberAccesExpression lub IndexExpression.

Może wymagać to dynamic_cast, ale dzięki temu nie będziemy musieli
się cofać parserem.

Ogólnie przekładamy ciężar weryfikacji, czy po lewej stronie
Expression jest typu do którego można przypisać wartość
na analizator semantyczny. Jednocześnie jednak otwiera to
potencjalnie możliwość chociażby na przypisywanie funkcji do zmiennych
(chociaż nie jest to obecnie wspierane, i wymagałoby zmiany leksera,
aby rozpoznował token typu funkcji).
////////////////////////////////////////////////////////////////////////////////

*/

class AssignmentStatement final : public Statement {
public:
   AssignmentStatement(SourceLocation location, ExprPtr target, ExprPtr value)
      : Statement(std::move(location)),
        target_(std::move(target)),
        value_(std::move(value)) {}

   const Expression& target() const {
      return *target_;
   }

   const Expression& value() const {
      return *value_;
   }

private:
   ExprPtr target_;
   ExprPtr value_;
};