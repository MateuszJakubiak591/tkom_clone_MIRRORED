#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "diagnostics/ErrorHandler.hpp"
#include "interpreter/Interpreter.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "source/FileSource.hpp"

namespace {
std::filesystem::path testDirectory() {
   return std::filesystem::current_path() / "interpreter_import_tests_tmp";
}

void writeFile(const std::filesystem::path& path, const std::string& content) {
   std::ofstream file(path);
   ASSERT_TRUE(file.good());
   file << content;
}

ProgramPtr parseFile(const std::filesystem::path& path, ErrorHandler& errorHandler) {
   FileSource source(path.string());
   Lexer lexer(source);
   Parser parser(lexer, &errorHandler);
   return parser.parseProgram();
}

int interpretFile(const std::filesystem::path& path, ErrorHandler& errorHandler) {
   ProgramPtr program = parseFile(path, errorHandler);
   if (errorHandler.hasErrors()) {
      return 1;
   }

   Interpreter interpreter(&errorHandler, nullptr, path.string());
   return interpreter.interpret(*program);
}

class InterpreterImportTests : public ::testing::Test {
protected:
   void SetUp() override {
      std::filesystem::remove_all(testDirectory());
      std::filesystem::create_directories(testDirectory());
   }

   void TearDown() override {
      std::filesystem::remove_all(testDirectory());
   }
};
}

TEST_F(InterpreterImportTests, CallsImportedFunctionAndReadsImportedConstantThroughMemberAccess) {
   const auto dir = testDirectory();
   writeFile(
      dir / "file1.djm",
      "int BASE = 8\n"
      "fun hidden() -> int {\n"
      "   return 3\n"
      "}\n"
      "fun fun1() -> int {\n"
      "   return BASE + hidden()\n"
      "}\n"
   );
   writeFile(
      dir / "main.djm",
      "import fun1, BASE from \"file1.djm\"\n"
      "fun main() -> int {\n"
      "   return file1.fun1() + file1.BASE\n"
      "}\n"
   );

   ErrorHandler errorHandler;
   EXPECT_EQ(interpretFile(dir / "main.djm", errorHandler), 19);
   EXPECT_FALSE(errorHandler.hasErrors());
}

TEST_F(InterpreterImportTests, DoesNotExposeImportedFunctionAsDirectCall) {
   const auto dir = testDirectory();
   writeFile(
      dir / "file1.djm",
      "fun fun1() -> int {\n"
      "   return 12\n"
      "}\n"
   );
   writeFile(
      dir / "main.djm",
      "import fun1 from \"file1.djm\"\n"
      "fun main() -> int {\n"
      "   return fun1()\n"
      "}\n"
   );

   ErrorHandler errorHandler;
   EXPECT_EQ(interpretFile(dir / "main.djm", errorHandler), 1);
   ASSERT_TRUE(errorHandler.hasErrors());
   EXPECT_EQ(errorHandler.errors().back().message, "function not found: fun1");
}

TEST_F(InterpreterImportTests, RejectsMemberAccessToNotImportedName) {
   const auto dir = testDirectory();
   writeFile(
      dir / "file1.djm",
      "int BASE = 8\n"
      "fun fun1() -> int {\n"
      "   return BASE\n"
      "}\n"
   );
   writeFile(
      dir / "main.djm",
      "import fun1 from \"file1.djm\"\n"
      "fun main() -> int {\n"
      "   return file1.BASE\n"
      "}\n"
   );

   ErrorHandler errorHandler;
   EXPECT_EQ(interpretFile(dir / "main.djm", errorHandler), 1);
   ASSERT_TRUE(errorHandler.hasErrors());
   EXPECT_EQ(errorHandler.errors().back().message, "imported member is not exported: file1.BASE");
}
