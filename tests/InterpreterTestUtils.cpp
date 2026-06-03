#include "InterpreterTestUtils.hpp"

#include <sstream>

#include "diagnostics/ErrorHandler.hpp"
#include "interpreter/Interpreter.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "source/StringSource.hpp"

InterpretationResult interpretSource(
   const std::string& code,
   const std::vector<std::string>& args,
   const std::string& filename
) {
   ErrorHandler errorHandler(code);
   StringSource source(filename, code);
   Lexer lexer(source);
   Parser parser(lexer, &errorHandler);
   ProgramPtr program = parser.parseProgram();

   std::ostringstream output;
   int exitCode = 1;

   if (program != nullptr && !errorHandler.hasErrors()) {
      Interpreter interpreter(&errorHandler, &output, filename);
      exitCode = interpreter.interpret(*program, args);
   }

   return InterpretationResult{
      exitCode,
      output.str(),
      errorHandler.errors()
   };
}
