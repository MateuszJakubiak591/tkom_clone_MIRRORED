#pragma once

#include <memory>
#include <vector>

#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"

#include "syntax/Expressions.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Type.hpp"
#include "syntax/Declarations.hpp"

#include "diagnostics/ErrorHandler.hpp"

#include "lexer/TokenSource.hpp"

class Parser {
public:
   explicit Parser(TokenSource& tokenSource, ErrorHandler* errorHandler = nullptr);

   ProgramPtr parseProgram();


private:
   //Lexer& lexer_;
   static NullErrorHandler nullHandler_;
   TokenSource& tokenSource_;

   Token current_;
   Token next_;
   Token previous_;

   ErrorHandler* errorHandler_;

   void advance();

   bool check(TokenType type) const;
   bool match(TokenType type);
   bool matchNewline();

   Token consume(TokenType type, const std::string& message);

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
   
   StmtPtr tryParseReturnStatement();
   StmtPtr tryParseContinueStatement();
   StmtPtr tryParseBreakStatement();
   StmtPtr parseVariableDeclarationStatement();

   StmtPtr tryParseIfStatement();
   StmtPtr tryParseWhileStatement();
   StmtPtr tryParseForStatement();

   void parseTopLevelDeclaration(
      std::vector<GlobalConstDeclPtr>& globalConstantDeclarations,
      std::vector<FunctionDeclPtr>& functionDeclarations
   );

   ImportDeclPtr parseImportDeclaration();

   GlobalConstDeclPtr tryParseGlobalConstantDeclaration();

   std::vector<ParameterNode> parseParameterList();
   ParameterNode parseParameter();

   std::unique_ptr<BlockStatement> parseBlock();

   ExprPtr parseExpression();

   StmtPtr parseStatement();
   StmtPtr tryParseBlockStatement();

   FunctionDeclPtr tryParseFunctionDeclaration();
};

class ParseError : public std::runtime_error {
   public:
      ParseError() : std::runtime_error("Parser error") {}
   };
