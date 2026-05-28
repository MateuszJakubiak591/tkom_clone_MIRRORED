#include "parser/Parser.hpp"

#include <sstream>
#include <utility>

#include "diagnostics/ErrorHandler.hpp"
#include "diagnostics/Error.hpp"
#include "syntax/Declarations.hpp"

#include <variant>

NullErrorHandler Parser::nullHandler_{};

Parser::Parser(TokenSource& tokenSource, ErrorHandler* errorHandler)
   : tokenSource_(tokenSource),
     current_(makeToken(TokenType::EndOfFile, "", SourceLocation{})),
     next_(makeToken(TokenType::EndOfFile, "", SourceLocation{})),
     previous_(makeToken(TokenType::EndOfFile, "", SourceLocation{})),
     errorHandler_(errorHandler) {

   if (errorHandler == nullptr) {
      errorHandler_ = &nullHandler_;
   } else {
      errorHandler_ = errorHandler;
   }
   current_ = tokenSource_.nextToken();
   next_ = tokenSource_.nextToken();
}

void Parser::advance() {
   previous_ = current_;
   current_ = next_;
   next_ = tokenSource_.nextToken();

   if (current_.type() == TokenType::Invalid) {
      errorHandler_->report(
         ErrorType::Lexical,
         "invalid token: " + current_.lexeme(),
         current_.location()
      );

      throw ParseError();
   }
}

bool Parser::check(TokenType type) const {
   return current_.type() == type;
}

// Jeśli token pasuje, to zjada, jeśli nie, nie rzuca wyjątku
bool Parser::match(TokenType type) {
   if (!check(type)) {
      return false;
   }

   advance();
   return true;
}

// Jeśli token pasuje, to zjada, jeśli nie, to rzuca wyjątek
// w przeciwieńśtwie do match, które wyjątku nie rzuca
// Istniał wcześniej bug, gdzie przy const Token& Parser::consume
// przez zwracanie referencji do previous_ był błąd przy std::get dla std::variant,
// bo nadpisano previous_ po consume() a przed std::get
Token Parser::consume(TokenType type, const std::string& message) {
   if (!check(type)) {
      errorHandler_->report(ErrorType::Parser, message, current_.location());


      //throw std::runtime_error(message);
      throw ParseError();
   }

   advance();
   return previous_;
}

void Parser::consumeStatementEnd() {
   if (match(TokenType::Newline)) {
      skipNewlines();
      return;
   }

   if (check(TokenType::RBrace) || check(TokenType::EndOfFile)) {
      return;
   }

   errorHandler_->report(
      ErrorType::Parser,
      "expected end of statement",
      current_.location()
   );

   //throw std::runtime_error("expected end of statement");
}

bool Parser::isAtEnd() const {
   return current_.type() == TokenType::EndOfFile;
}

bool Parser::matchNewline() {
   return match(TokenType::Newline);
}

void Parser::skipNewlines() {
   while (match(TokenType::Newline)) {}
}

bool Parser::isBasicValueTypeStart() const {
return check(TokenType::KwInt)
      || check(TokenType::KwUint)
      || check(TokenType::KwFloat)
      || check(TokenType::KwBool)
      || check(TokenType::KwChar)
      || check(TokenType::KwString);
}

bool Parser::isValueTypeStart() const {
return isBasicValueTypeStart()
      || check(TokenType::KwList);
      //|| check(TokenType::Identifier);
}

std::unique_ptr<TypeNode> Parser::parseType() {
   // !!! Trzeba wyróżnić void, i nie umieszczać go
   // w parseValueType
   if (check(TokenType::KwVoid)) {
      SourceLocation location = current_.location();
      advance();
      return std::make_unique<VoidTypeNode>(location);
   }

   return parseValueType();
}

std::unique_ptr<TypeNode> Parser::parseValueType() {
   SourceLocation location = current_.location();

   if (match(TokenType::KwInt)) {
      return std::make_unique<IntTypeNode>(location);
   }

   if (match(TokenType::KwUint)) {
      return std::make_unique<UintTypeNode>(location);
   }

   if (match(TokenType::KwFloat)) {
      return std::make_unique<FloatTypeNode>(location);
   }

   if (match(TokenType::KwBool)) {
      return std::make_unique<BoolTypeNode>(location);
   }

   if (match(TokenType::KwChar)) {
      return std::make_unique<CharTypeNode>(location);
   }

   if (match(TokenType::KwString)) {
      return std::make_unique<StringTypeNode>(location);
   }

   if (match(TokenType::KwList)) {
      consume(TokenType::Less, "expected '<' after list");

      auto elementType = parseValueType();

      consume(TokenType::Greater, "expected '>' after list element type");

      return std::make_unique<ListTypeNode>(location, std::move(elementType));
   }

   errorHandler_->report(ErrorType::Parser, "expected value type",
                                                current_.location());

   throw ParseError();
}

/*
expression = logical_or_expr ;
*/
ExprPtr Parser::parseExpression() {
   return parseLogicalOrExpression();
}

/*
logical_or_expr = logical_and_expr ,
                  { "||" , logical_and_expr } ;
*/
ExprPtr Parser::parseLogicalOrExpression() {
   auto expression = parseLogicalAndExpression();

   while (check(TokenType::LogicOr)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseLogicalAndExpression();

      expression = std::make_unique<LogicalOrExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
logical_and_expr
                = equality_expr ,
                  { "&&" , equality_expr } ;
*/
ExprPtr Parser::parseLogicalAndExpression() {
   auto expression = parseEqualityExpression();

   while (check(TokenType::LogicAnd)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseEqualityExpression();

      expression = std::make_unique<LogicalAndExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
equality_expr   = relational_expr ,
                  [ ( "==" | "!=" | "contains" ) , relational_expr ] ;
*/
ExprPtr Parser::parseEqualityExpression() {
   auto expression = parseRelationalExpression();

   if (check(TokenType::Equal)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseRelationalExpression();

      return std::make_unique<EqualExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   if (check(TokenType::NotEqual)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseRelationalExpression();

      return std::make_unique<NotEqualExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   if (check(TokenType::OpContains)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseRelationalExpression();

      return std::make_unique<ContainsExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
relational_expr = additive_expr ,
                  [ ( "<" | "<=" | ">" | ">=" ) , additive_expr ] ;
*/
ExprPtr Parser::parseRelationalExpression() {
   auto expression = parseAdditiveExpression();

   if (check(TokenType::Less)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseAdditiveExpression();

      return std::make_unique<LessExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   if (check(TokenType::LessEqual)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseAdditiveExpression();

      return std::make_unique<LessEqualExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   if (check(TokenType::Greater)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseAdditiveExpression();

      return std::make_unique<GreaterExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   if (check(TokenType::GreaterEqual)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseAdditiveExpression();

      return std::make_unique<GreaterEqualExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
additive_expr   = multiplicative_expr ,
                  { ( "+" | "-" ) , multiplicative_expr } ;
*/
ExprPtr Parser::parseAdditiveExpression() {
   auto expression = parseMultiplicativeExpression();

   while (check(TokenType::Plus) || check(TokenType::Minus)) {
      SourceLocation location = current_.location();

      if (match(TokenType::Plus)) {
         auto right = parseMultiplicativeExpression();

         expression = std::make_unique<AddExpression>(
            location,
            std::move(expression),
            std::move(right)
         );
      } else {
         //consume(TokenType::Minus, "expected '-'");
         advance();

         auto right = parseMultiplicativeExpression();

         expression = std::make_unique<SubtractExpression>(
            location,
            std::move(expression),
            std::move(right)
         );
      }
   }

   return expression;
}

/*
multiplicative_expr
                = list_operator_expr ,
                  { ( "*" | "/" ) , list_operator_expr } ;
*/
ExprPtr Parser::parseMultiplicativeExpression() {
   auto expression = parseMapExpression();

   while (check(TokenType::Multiply) || check(TokenType::Divide)) {
      SourceLocation location = current_.location();

      if (match(TokenType::Multiply)) {
         auto right = parseMapExpression();

         expression = std::make_unique<MultiplyExpression>(
            location,
            std::move(expression),
            std::move(right)
         );
      } else {
         //consume(TokenType::Divide, "expected '/'");
         advance();

         auto right = parseMapExpression();

         expression = std::make_unique<DivideExpression>(
            location,
            std::move(expression),
            std::move(right)
         );
      }
   }

   return expression;
}

/*
list_operator_expr
                = map_expr ;

map_expr        = filter_expr ,
                  { "|>" , filter_expr } ;
*/
ExprPtr Parser::parseMapExpression() {
   auto expression = parseFilterExpression();

   while (check(TokenType::OpMap)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseFilterExpression();

      expression = std::make_unique<MapExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
filter_expr     = group_expr ,
                  { "?" , group_expr } ;
*/
ExprPtr Parser::parseFilterExpression() {
   auto expression = parseGroupExpression();

   while (check(TokenType::OpFilter)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseGroupExpression();

      expression = std::make_unique<FilterExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
group_expr      = cast_expr ,
                  { "%" , cast_expr } ;
*/
ExprPtr Parser::parseGroupExpression() {
   auto expression = parseCastExpression();

   while (check(TokenType::OpGroup)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseCastExpression();

      expression = std::make_unique<GroupExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
cast_expr       = unary_expr ,
                  { "as" , value_type } ;
*/
ExprPtr Parser::parseCastExpression() {
   auto expression = parseUnaryExpression();

   while (check(TokenType::OpAs)) {
      SourceLocation location = current_.location();
      advance();

      auto targetType = parseValueType();

      expression = std::make_unique<CastExpression>(
         location,
         std::move(expression),
         std::move(targetType)
      );
   }

   return expression;
}

/*
unary_expr      = unary_op , unary_expr
                | power_expr ;
*/
ExprPtr Parser::parseUnaryExpression() {
   SourceLocation location = current_.location();

   if (match(TokenType::Minus)) {
      auto operand = parseUnaryExpression();
      return std::make_unique<NegateExpression>(location, std::move(operand));
   }

   if (match(TokenType::Not)) {
      auto operand = parseUnaryExpression();
      return std::make_unique<NotExpression>(location, std::move(operand));
   }

   if (match(TokenType::OpCount)) {
      auto operand = parseUnaryExpression();
      return std::make_unique<CountExpression>(location, std::move(operand));
   }

   if (match(TokenType::OpReverse)) {
      auto operand = parseUnaryExpression();
      return std::make_unique<ReverseExpression>(location, std::move(operand));
   }

   if (match(TokenType::OpFlatten)) {
      auto operand = parseUnaryExpression();
      return std::make_unique<FlattenExpression>(location, std::move(operand));
   }

   return parsePowerExpression();
}

/*
power_expr      = postfix_expr ,
                  [ "^" , unary_expr ] ;
*/
ExprPtr Parser::parsePowerExpression() {
   auto expression = parsePostfixExpression();

   if (check(TokenType::Power)) {
      SourceLocation location = current_.location();
      advance();

      auto right = parseUnaryExpression();

      return std::make_unique<PowerExpression>(
         location,
         std::move(expression),
         std::move(right)
      );
   }

   return expression;
}

/*
postfix_expr    = primary_expr , { postfix_op } ;
postfix_op      = 	"." , identifier , [ call_suffix ]
                	| index_suffix
               		| slice_suffix
                	| call_suffix ;
*/
ExprPtr Parser::parsePostfixExpression() {
   auto expression = parsePrimaryExpression();

   while (true) {

      if (match(TokenType::Dot)) {
         Token memberName = consume(
            TokenType::Identifier,
            "expected member name after '.'"
         );

         expression = std::make_unique<MemberAccessExpression>(
            memberName.location(),
            std::move(expression),
            memberName.lexeme()
         );

         continue;
      }

      if (check(TokenType::LParen)) {
         SourceLocation location = current_.location();

         auto arguments = parseArgumentList();

         expression = std::make_unique<CallExpression>(
            location,
            std::move(expression),
            std::move(arguments)
         );

         continue;
      }

      if (match(TokenType::LBracket)) {
         SourceLocation location = previous_.location();

         if (match(TokenType::Colon)) {
            ExprPtr end = nullptr;

            if (!check(TokenType::RBracket)) {
               end = parseExpression();
            }

            consume(TokenType::RBracket, "expected ']' after slice");

            expression = std::make_unique<SliceExpression>(
               location,
               std::move(expression),
               nullptr,
               std::move(end)
            );

            continue;
         }

         auto startOrIndex = parseExpression();

         if (match(TokenType::Colon)) {
            ExprPtr end = nullptr;

            if (!check(TokenType::RBracket)) {
               end = parseExpression();
            }

            consume(TokenType::RBracket, "expected ']' after slice");

            expression = std::make_unique<SliceExpression>(
               location,
               std::move(expression),
               std::move(startOrIndex),
               std::move(end)
            );

            continue;
         }

         consume(TokenType::RBracket, "expected ']' after index expression");

         expression = std::make_unique<IndexExpression>(
            location,
            std::move(expression),
            std::move(startOrIndex)
         );

         continue;
      }

      break;
   }

   return expression;
}

/*
argument_list   = expression , { "," , expression } ;
*/
std::vector<ExprPtr> Parser::parseArgumentList() {
   consume(TokenType::LParen, "expected '('");
   // Teoretycznie nie powinno się we wcześniejszym kodzie
   // dopuścić do sytuacji, że wywołujemy powyższe parse_XXX
   // bez sprawdzenia check(Token:LParen), ale w razie czego
   // tak napisałem (gdyby wywołać powyższe parse_XXX bez uprzedniego sprawdzenia)

   std::vector<ExprPtr> arguments;

   if (!check(TokenType::RParen)) {
      do {
         arguments.push_back(parseExpression());
      } while (match(TokenType::Comma));
   }

   consume(TokenType::RParen, "expected ')' after argument list");

   return arguments;
}

/*
primary_expr    = literal
                | identifier
                | "(" , expression , ")"
                | "this" ;
*/
ExprPtr Parser::parsePrimaryExpression() {
   SourceLocation location = current_.location();

   if (check(TokenType::IntLiteral)) {
      int64_t value = std::get<int64_t>(current_.value());
      advance();

      return std::make_unique<IntLiteralExpression>(location, value);
   }

   if (check(TokenType::FloatLiteral)) {
      double value = std::get<double>(current_.value());
      advance();

      return std::make_unique<FloatLiteralExpression>(location, value);
   }

   if (check(TokenType::BoolLiteral)) {
      bool value = std::get<bool>(current_.value());
      advance();

      return std::make_unique<BoolLiteralExpression>(location, value);
   }

   if (check(TokenType::CharLiteral)) {
      char value = std::get<char>(current_.value());
      advance();

      return std::make_unique<CharLiteralExpression>(location, value);
   }

   if (check(TokenType::StringLiteral)) {
      std::string value = std::get<std::string>(current_.value());
      advance();

      return std::make_unique<StringLiteralExpression>(
         location,
         std::move(value)
      );
   }

   if (check(TokenType::LBracket)) {
      return parseListLiteralExpression(location);
   }

   if (check(TokenType::Identifier)) {
      std::string name = current_.lexeme();
      advance();

      return std::make_unique<IdentifierExpression>(
         location,
         std::move(name)
      );
   }

   if (match(TokenType::KwThis)) {
      return std::make_unique<ThisExpression>(location);
   }

   if (match(TokenType::LParen)) {
      auto expression = parseExpression();

      consume(TokenType::RParen, "expected ')' after expression");

      return expression;
   }

   errorHandler_->report(
      ErrorType::Parser,
      "expected expression",
      current_.location()
   );

   throw ParseError();
}

/*
list_literal    = "[" , [ expression , { "," , expression } ] , "]" ;
*/
ExprPtr Parser::parseListLiteralExpression(SourceLocation location) {
   consume(TokenType::LBracket, "expected '['");

   std::vector<ExprPtr> elements;

   if (!check(TokenType::RBracket)) {
      do {
         elements.push_back(parseExpression());
      } while (match(TokenType::Comma));
   }

   consume(TokenType::RBracket, "expected ']' after list literal");

   return std::make_unique<ListLiteralExpression>(
      location,
      std::move(elements)
   );
}

/*
statement       = variable_decl_stmt
                | expression_or_assignment_stmt
                | return_stmt
                | if_stmt
                | while_stmt
                | for_stmt
                | block ;
*/
StmtPtr Parser::parseStatement() {
   try {
      skipNewlines();

      if (isAtEnd()) {
         return nullptr;
      }

      if (auto statement = tryParseBlockStatement()) {
         return statement;
      }

      if (auto statement = tryParseReturnStatement()) {
         return statement;
      }

      if (auto statement = tryParseContinueStatement()) {
         return statement;
      }

      if (auto statement = tryParseBreakStatement()) {
         return statement;
      }

      if (auto statement = tryParseIfStatement()) {
         return statement;
      }

      if (auto statement = tryParseWhileStatement()) {
         return statement;
      }

      if (auto statement = tryParseForStatement()) {
         return statement;
      }

      if (check(TokenType::KwMut) || isValueTypeStart()) {
         return parseVariableDeclarationStatement();
      }

      return parseExpressionOrAssignmentStatement();
   } 
   catch (const ParseError& e) {
      // Synchronizacja
      advance(); 
      while (!isAtEnd() && previous_.type() != TokenType::Newline) {
         advance();
      }
      return nullptr; 
   }
}

/*
if_stmt         = "if" , expression , block ,
                  { "else" , "if" , expression , block } ,
                  [ "else" , block ] ;
*/
StmtPtr Parser::tryParseIfStatement() {
   if (!check(TokenType::KwIf)) {
      return nullptr;
   }

   advance();
   SourceLocation location = previous_.location();

   auto condition = parseExpression();

   auto thenBranch = parseBlock();

   StmtPtr elseBranch = nullptr;

   if (match(TokenType::KwElse)) {
      if (check(TokenType::KwIf)) {
         elseBranch = tryParseIfStatement();
      } else {
         elseBranch = parseBlock();
      }
   }

   return std::make_unique<IfStatement>(
      location,
      std::move(condition),
      std::move(thenBranch),
      std::move(elseBranch)
   );
}

/*
while_stmt      = "while" , expression , block ;
*/
StmtPtr Parser::tryParseWhileStatement() {
   if (!check(TokenType::KwWhile)) {
      return nullptr;
   }

   advance();
   SourceLocation location = previous_.location();

   auto condition = parseExpression();

   auto body = parseBlock();

   return std::make_unique<WhileStatement>(
      location,
      std::move(condition),
      std::move(body)
   );
}

/*
for_stmt        = "for" , value_type , identifier , "in" , expression , block ;
*/
StmtPtr Parser::tryParseForStatement() {
   if (!check(TokenType::KwFor)) {
      return nullptr;
   }

   advance();
   SourceLocation location = previous_.location();

   Token variableToken = consume(
      TokenType::Identifier,
      "expected loop variable name after 'for'"
   );

   consume(TokenType::KwIn, "expected 'in' after loop variable");

   auto iterable = parseExpression();

   auto body = parseBlock();

   return std::make_unique<ForStatement>(
      location,
      variableToken.lexeme(),
      variableToken.location(),
      std::move(iterable),
      std::move(body)
   );
}

/*
block           = "{" , { statement } , "}" ;
*/
std::unique_ptr<BlockStatement> Parser::parseBlock() {
   SourceLocation location = current_.location();

   consume(TokenType::LBrace, "expected '{'");
   // Teoretycznie nie powinno się we wcześniejszym kodzie
   // dopuścić do sytuacji, że wywołujemy powyższe parse_XXX
   // bez sprawdzenia check(Token:LBrace), ale w razie czego
   // tak napisałem (gdyby wywołać powyższe parse_XXX bez uprzedniego sprawdzenia)

   skipNewlines();

   std::vector<StmtPtr> statements;

   while (!check(TokenType::RBrace) && !isAtEnd()) {
      statements.push_back(parseStatement());
      skipNewlines();
   }

   consume(TokenType::RBrace, "expected '}' after block");

   if (check(TokenType::Newline)) {
      advance();
   }

   return std::make_unique<BlockStatement>(
      location,
      std::move(statements)
   );
}

StmtPtr Parser::tryParseBlockStatement() {
   if (!check(TokenType::LBrace)) {
      return nullptr;
   }

   return parseBlock();
}

/*
variable_decl_stmt	= [ "mut" ] , 	value_type , identifier_list , [ "=" , expression ] ;
*/
StmtPtr Parser::parseVariableDeclarationStatement() {
   SourceLocation location = current_.location();

   bool isMutable = false;

   if (match(TokenType::KwMut)) {
      isMutable = true;
   }

   auto type = parseValueType();

   std::vector<VariableDeclarator> names;

   const Token& firstName = consume(
      TokenType::Identifier,
      "expected variable name"
   );

   names.push_back(VariableDeclarator{
      firstName.lexeme(),
      firstName.location()
   });

   while (match(TokenType::Comma)) {
      const Token& name = consume(
         TokenType::Identifier,
         "expected variable name after ','"
      );

      names.push_back(VariableDeclarator{
         name.lexeme(),
         name.location()
      });
   }

   ExprPtr initializer = nullptr;

   if (match(TokenType::Assign)) {
      initializer = parseExpression();
   }

   consumeStatementEnd();

   return std::make_unique<VariableDeclarationStatement>(
      location,
      isMutable,
      std::move(type),
      std::move(names),
      std::move(initializer)
   );
}

/*
expression_or_assignment_stmt
                = expression , [ "=" , expression ] ;
*/
StmtPtr Parser::parseExpressionOrAssignmentStatement() {
   SourceLocation location = current_.location();

   ExprPtr left = parseExpression();

   if (match(TokenType::Assign)) {
      if (!isAssignable(*left)) {
         errorHandler_->report(
            ErrorType::Parser,
            "left side of assignment is not assignable",
            left->location()
         );

         throw std::runtime_error("left side of assignment is not assignable");
      }

      ExprPtr value = parseExpression();

      consumeStatementEnd();

      return std::make_unique<AssignmentStatement>(
         location,
         std::move(left),
         std::move(value)
      );
   }

   consumeStatementEnd();

   return std::make_unique<ExpressionStatement>(
      location,
      std::move(left)
   );
}

/*
To może nie jest idealne, sprawdza tylko to
co jest po prawej stronie, po ostatniej kropce, np.

user.x = 5
przejdzie, ale

user.getName() = 5
nie przejdzie, bo

CallExpression
  callee:
    MemberAccessExpression("getName")
      object:
        IdentifierExpression("user")

Oczywiście gdy zrobimy
user.getName = 5
bez nawiasów, to też przejdzie, ale to już niech
wykryje analizator semantyczny
*/
bool Parser::isAssignable(const Expression& expression) const {
   if (dynamic_cast<const IdentifierExpression*>(&expression) != nullptr) {
      return true;
   }

   if (dynamic_cast<const MemberAccessExpression*>(&expression) != nullptr) {
      return true;
   }

   if (dynamic_cast<const IndexExpression*>(&expression) != nullptr) {
      return true;
   }

   return false;
}

StmtPtr Parser::tryParseContinueStatement() {
   if (!check(TokenType::KwContinue)) {
      return nullptr;
   }

   SourceLocation location = current_.location();
   advance();
   consumeStatementEnd();
   return std::make_unique<ContinueStatement>(location);
}

StmtPtr Parser::tryParseBreakStatement() {
   if (!check(TokenType::KwBreak)) {
      return nullptr;
   }

   SourceLocation location = current_.location();
   advance();
   consumeStatementEnd();
   return std::make_unique<BreakStatement>(location);
}
/*
return_stmt     = "return" , [ expression ] ;
*/
StmtPtr Parser::tryParseReturnStatement() {
   if (!check(TokenType::KwReturn)) {
      return nullptr;
   }

   SourceLocation location = current_.location();

   consume(TokenType::KwReturn, "expected 'return'");
   ExprPtr expression = nullptr;

   if (!check(TokenType::Newline) &&
       !check(TokenType::RBrace) &&
       !check(TokenType::EndOfFile)) {
      expression = parseExpression();
   }

   consumeStatementEnd();

   return std::make_unique<ReturnStatement>(
      location,
      std::move(expression)
   );
}

ProgramPtr Parser::parseProgram() {
   SourceLocation location = current_.location();

   skipNewlines();

   std::vector<ImportDeclPtr> imports;

   while (auto declaration = parseImportDeclaration()) {
      imports.push_back(std::move(declaration));
      skipNewlines();
   }

   std::vector<DeclPtr> declarations;

   while (!isAtEnd()) {
      skipNewlines();

      if (isAtEnd()) {
         break;
      }

      declarations.push_back(parseTopLevelDeclaration());
      skipNewlines();
   }

   return std::make_unique<Program>(
      location,
      std::move(imports),
      std::move(declarations)
   );
}

/*
top_level_decl  = 
                | function_decl
                | global_const_decl ;
*/
DeclPtr Parser::parseTopLevelDeclaration() {
   if (auto declaration = tryParseFunctionDeclaration()) {
      return declaration;
   }

   if (auto declaration = tryParseGlobalConstantDeclaration()) {
      return declaration;
   }

   errorHandler_->report(
      ErrorType::Parser,
      "expected top-level declaration",
      current_.location()
   );

   throw ParseError();
}

/*
import_spec     = "*"
                | identifier , { "," , identifier } ;

import_decl     = "import" , import_spec , "from" , string_literal ;
*/
ImportDeclPtr Parser::parseImportDeclaration() {
   if (!check(TokenType::KwImport)) {
      return nullptr;
   }

   SourceLocation location = current_.location();

   consume(TokenType::KwImport, "expected 'import'");

   bool importAll = false;
   std::vector<std::string> names;

   if (match(TokenType::Multiply)) {
      importAll = true;
   } else {
      const Token& firstName = consume(
         TokenType::Identifier,
         "expected imported name or '*'"
      );

      names.push_back(firstName.lexeme());

      while (match(TokenType::Comma)) {
         const Token& name = consume(
            TokenType::Identifier,
            "expected imported name after ','"
         );

         names.push_back(name.lexeme());
      }
   }

   consume(TokenType::KwFrom, "expected 'from' after import spec");

   Token pathToken = consume(
      TokenType::StringLiteral,
      "expected string literal after 'from'"
   );

   consumeStatementEnd();

   std::string path = std::get<std::string>(pathToken.value());

   return std::make_unique<ImportDeclaration>(
      location,
      importAll,
      std::move(names),
      std::move(path)
   );
}

/*
parameter_list  = parameter , { "," , parameter } ;

parameter       = identifier , ":" , value_type ;
*/
ParameterNode Parser::parseParameter() {
   const Token& nameToken = consume(
      TokenType::Identifier,
      "expected parameter name"
   );

   consume(TokenType::Colon, "expected ':' after parameter name");

   auto type = parseValueType();

   return ParameterNode{
      nameToken.lexeme(),
      nameToken.location(),
      std::move(type)
   };
}

std::vector<ParameterNode> Parser::parseParameterList() {
   std::vector<ParameterNode> parameters;

   parameters.push_back(parseParameter());

   while (match(TokenType::Comma)) {
      parameters.push_back(parseParameter());
   }

   return parameters;
}

/*
function_decl 	= 	"fun" , identifier ,
                	"(" , [ parameter_list ] , ")" ,
                	"->" , type ,
                	block ;
*/
FunctionDeclPtr Parser::tryParseFunctionDeclaration() {
   if (!match(TokenType::KwFun)) {
      return nullptr;
   }

   SourceLocation location = previous_.location();

   const Token& nameToken = consume(
      TokenType::Identifier,
      "expected function name after 'fun'"
   );

   consume(TokenType::LParen, "expected '(' after function name");

   std::vector<ParameterNode> parameters;

   if (!check(TokenType::RParen)) {
      parameters = parseParameterList();
   }

   consume(TokenType::RParen, "expected ')' after parameter list");

   consume(TokenType::Arrow, "expected '->' after function parameters");

   auto returnType = parseType();

   auto body = parseBlock();

   return std::make_unique<FunctionDeclaration>(
      location,
      nameToken.lexeme(),
      std::move(parameters),
      std::move(returnType),
      std::move(body)
   );
}

/*
global_const_decl	= 		value_type , identifier_list , [ "=" , expression ] ;
*/
GlobalConstDeclPtr Parser::tryParseGlobalConstantDeclaration() {
   if (!isValueTypeStart()) {
      return nullptr;
   }

   SourceLocation location = current_.location();

   auto type = parseValueType();

   std::vector<VariableDeclarator> names;

   const Token& firstName = consume(
      TokenType::Identifier,
      "expected global constant name"
   );

   names.push_back(VariableDeclarator{
      firstName.lexeme(),
      firstName.location()
   });

   while (match(TokenType::Comma)) {
      const Token& name = consume(
         TokenType::Identifier,
         "expected global constant name after ','"
      );

      names.push_back(VariableDeclarator{
         name.lexeme(),
         name.location()
      });
   }

   ExprPtr initializer = nullptr;

   if (match(TokenType::Assign)) {
      initializer = parseExpression();
   }

   consumeStatementEnd();

   return std::make_unique<GlobalConstantDeclaration>(
      location,
      std::move(type),
      std::move(names),
      std::move(initializer)
   );
}

