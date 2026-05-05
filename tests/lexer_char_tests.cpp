#include "TestsUtils.hpp"
#include "lexer/Token.hpp"
#include <string>


static void testInvalidEmptyChar() {
   const std::string code =
      "char c = ''\n";

   const auto tokens = tokenizeString("invalid_char.djm", code);

   assertToken(tokens[0], TokenType::KwChar, "char", 1, 1);
   assertToken(tokens[1], TokenType::Identifier, "c", 1, 6);
   assertToken(tokens[2], TokenType::Assign, "=", 1, 8);
   assertToken(tokens[3], TokenType::Invalid, "'", 1, 10);
}

void runLexerCharTests(){
   runTest("InvalidEmptyChar", testInvalidEmptyChar);
}

int main(){
   runLexerCharTests();
   return 0;
}