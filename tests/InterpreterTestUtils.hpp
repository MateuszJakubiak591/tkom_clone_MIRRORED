#pragma once

#include <string>
#include <vector>

#include "diagnostics/Error.hpp"

struct InterpretationResult {
   int exitCode = 0;
   std::string output;
   std::vector<Error> errors;
};

InterpretationResult interpretSource(
   const std::string& code,
   const std::vector<std::string>& args = {},
   const std::string& filename = "interpreter_test.djm"
);
