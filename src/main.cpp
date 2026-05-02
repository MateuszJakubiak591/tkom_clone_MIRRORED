#include "source/StringSource.hpp"
#include "source/FileSource.hpp"
#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"

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

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "Użycie: " << argv[0] << " <plik.djm>\n";
      return 1;
   }

   const std::string path = argv[1];

   try {
      //std::string content = readFileToString(path);
      //StringSource source(path, content);
      FileSource fileSource(path);

      Lexer lexer(fileSource);

      while (true) {
         Token token = lexer.nextToken();

         std::cout
            << token.location.filename << ":"
            << token.location.line << ":"
            << token.location.column << "  "
            << tokenTypeToString(token.type)
            << "  \"" << token.lexeme << "\"\n";

         if (token.type == TokenType::EndOfFile) {
            break;
         }

         if (token.type == TokenType::Invalid) {
            std::cerr << "Niepoprawny token: " << token.lexeme << "\n";
         }
      }
   } catch (const std::exception& e) {
      std::cerr << "Błąd: " << e.what() << "\n";
      return 1;
   }

   return 0;
}