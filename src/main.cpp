#include "source/StringSource.hpp"
#include "source/FileSource.hpp"
#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
#include "source/SourceLocation.hpp"
#include "parser/Parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static std::string readFileToString(const std::string& path) {
   std::ifstream file(path);


   if (!file) {
      throw std::runtime_error("Nie można otworzyć pliku: " + path);
   }

   std::ostringstream buffer;
   buffer << file.rdbuf();
   return buffer.str();
}

StmtPtr parseString(const std::string& code, ErrorHandler* errHandler) {
   auto source = std::make_unique<StringSource>("test.djm", code);
   Lexer lexer(*source);
   Parser parser(lexer, errHandler);
   
   return parser.parseStatement();
}

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "Użycie: " << argv[0] << " <plik.djm>\n";
      return 1;
   }

   //SourceLocation sc = SourceLocation{"file1.djm", 5, 10};
   //makeBoolToken("true1", sc, true);

   const std::string path = argv[1];

   try {
      //std::string content = readFileToString(path);
      //StringSource source(path, content);
      FileSource fileSource(path);
      Lexer lexer(fileSource);
      ErrorHandler errHandler(readFileToString(path));
      Parser parser(lexer, &errHandler);

      StmtPtr parsedStatements = parser.parseBlockStatement();

      errHandler.printErrors(std::cout);
   } catch (const std::exception& e) {
      std::cerr << "Błąd: " << e.what() << "\n";
      return 1;
   }    
}



















////////////////////////////////////////////////////////////////////////////////////////
/*

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "Użycie: " << argv[0] << " <plik.djm>\n";
      return 1;
   }

   //SourceLocation sc = SourceLocation{"file1.djm", 5, 10};
   //makeBoolToken("true1", sc, true);

   const std::string path = argv[1];

   try {
      //std::string content = readFileToString(path);
      //StringSource source(path, content);
      FileSource fileSource(path);

      Lexer lexer(fileSource);

      int i = 0;

      while (true) {
         Token token = lexer.nextToken();

         std::cout
            << "assertToken(tokens["
            << std::to_string(i)
            << "], TokenType::"
            << tokenTypeToString(token.type());

         if(token.type() == TokenType::StringLiteral){
            std::string s = token.lexeme();
            s.pop_back();
            std::cout << ", \"\\" << s << "\\\"\"";   "\\n";
         } else if(token.type() == TokenType::Newline){
            std::cout << ", \"\\\\n\"";
         }
         else {
            std::cout << ", \"" << token.lexeme() << "\"";
         }

         std::cout
            << ", " << token.location().line
            << ", " << token.location().column
            << ");\n";

         if (token.type() == TokenType::Newline) {
            std::cout << "\n";
         }

         if (token.type() == TokenType::EndOfFile) {
            break;
         }

         if (token.type() == TokenType::Invalid) {
            std::cerr << "Niepoprawny token: " << token.lexeme() << "\n";
         }

         i++;
      }
   } catch (const std::exception& e) {
      std::cerr << "Błąd: " << e.what() << "\n";
      return 1;
   }

   return 0;
}

*/