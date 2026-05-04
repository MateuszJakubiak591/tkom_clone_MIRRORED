#include <iostream>

void runLexerKeywordTests();
void runLexerStringTests();
void runLexerFileTests();

int main() {
   runLexerKeywordTests();
   runLexerStringTests();
   runLexerFileTests();

   std::cout << "All lexer tests passed.\n";
   return 0;
}