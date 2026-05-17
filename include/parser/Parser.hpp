#pragma once

#include <memory>
#include <vector>

#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"

#include "syntax/Expressions.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Type.hpp"

#include "diagnostics/ErrorHandler.hpp"

class Parser {
public:
   explicit Parser(Lexer& lexer, ErrorHandler* errorHandler = nullptr);

   ExprPtr parseExpression();
   StmtPtr parseStatement();

private:
   Lexer& lexer_;

   Token current_;
   Token next_;
   Token previous_;

   ErrorHandler* errorHandler_;

   void advance();

   bool check(TokenType type) const;
   bool match(TokenType type);
   bool matchNewline();

   const Token& consume(TokenType type, const std::string& message);

   void skipNewlines();
   void consumeStatementEnd();

   bool isAtEnd() const;

   bool isValueTypeStart() const;
   bool isBasicValueTypeStart() const;
   bool looksLikeVariableDeclaration() const;

   bool isAssignable(const Expression& expression) const;
   bool isAssignableObject(const Expression& expression) const;

   std::unique_ptr<TypeNode> parseType();
   std::unique_ptr<TypeNode> parseValueType();

   ExprPtr parseLogicalOrExpression();
   ExprPtr parseLogicalAndExpression();
   ExprPtr parseEqualityExpression();
   ExprPtr parseRelationalExpression();
   ExprPtr parseAdditiveExpression();
   ExprPtr parseMultiplicativeExpression();
   ExprPtr parseMapExpression();
   ExprPtr parseFilterExpression();
   ExprPtr parseGroupExpression();
   ExprPtr parseCastExpression();
   ExprPtr parseUnaryExpression();
   ExprPtr parsePowerExpression();
   ExprPtr parsePostfixExpression();
   ExprPtr parsePrimaryExpression();
   ExprPtr parseListLiteralExpression(SourceLocation location);

   std::vector<ExprPtr> parseArgumentList();

   StmtPtr parseExpressionOrAssignmentStatement();
   StmtPtr parseBlockStatement();
   StmtPtr parseReturnStatement();
   StmtPtr parseVariableDeclarationStatement();
};