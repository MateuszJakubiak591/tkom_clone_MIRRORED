#include <gtest/gtest.h>
#include "TestsUtils.hpp"
#include "syntax/Statements.hpp"
#include "syntax/Expressions.hpp"

TEST(ParserTests, ParseVariableDeclaration) {
   // 1. Arrange: Kod wejściowy
   std::string code = "mut int x = 42\n";
   ErrorHandler errHandler(""); // Przekazujemy pusty string, byle łapał błędy

   // 2. Act: Odpalenie parsera
   StmtPtr stmt = parseString(code, &errHandler);

   // 3. Assert: Weryfikacja błędów
   EXPECT_FALSE(errHandler.hasErrors()) << "Parser zgłosił błędy składniowe!";
   ASSERT_NE(stmt, nullptr) << "Parser zwrócił nullptr!";

   // 4. Assert: Weryfikacja typu instrukcji
   // Używamy dynamic_cast na gołym wskaźniku (.get()), żeby sprawdzić typ
   auto* varDecl = dynamic_cast<VariableDeclarationStatement*>(stmt.get());
   ASSERT_NE(varDecl, nullptr) << "Instrukcja nie jest VariableDeclarationStatement!";

   // 5. Assert: Weryfikacja detali węzła
   EXPECT_TRUE(varDecl->isMutable());
   
   // Sprawdzanie typu zmiennej (powinien być IntTypeNode)
   auto* intType = dynamic_cast<const IntTypeNode*>(&varDecl->type());
   ASSERT_NE(intType, nullptr) << "Typ zmiennej to nie jest 'int'!";

   // Sprawdzanie nazwy zmiennej
   ASSERT_EQ(varDecl->names().size(), 1);
   EXPECT_EQ(varDecl->names()[0].name, "x");

   // Sprawdzanie wyrażenia inicjalizującego (= 42)
   ASSERT_NE(varDecl->initializer(), nullptr);
   auto* initExpr = dynamic_cast<const IntLiteralExpression*>(varDecl->initializer());
   ASSERT_NE(initExpr, nullptr) << "Inicjalizator nie jest literałem całkowitym!";
   EXPECT_EQ(initExpr->value(), 42);
}